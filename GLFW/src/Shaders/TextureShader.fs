#version 400 core
out vec4 FragColor;
in vec3 outPos;
in vec3 outNorm;
in vec2 outTex;
in mat3 outTBN;

struct Material {
    sampler2D diffuse0;
    sampler2D specular0;
    sampler2D normal0;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool hasDiffuseMap;
    bool hasSpecularMap;
    bool hasNormalMap;
    float shininess;
};

struct Light {
    int type; // 0: directional, 1: point, 2: spot
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;

    // Shadow mapping. -1 means "no shadow map assigned this frame" -- skip the lookup.
    int shadowMapLayer;     // directional/spot: layer index into shadowMapArray
    mat4 lightSpaceMatrix;  // directional/spot: projects world space into that layer's UV+depth

    int shadowCubeLayer;    // point: light-slot index into pointShadowMapArray (layer-face = slot*6+face)
    float shadowFarPlane;   // point: far-plane distance used to decode the stored linear distance
};

uniform Material material;
uniform Light[16] lights;

uniform vec3 ambientLight;
uniform int numLights;
uniform vec3 viewPos;

// Every directional/spot light that casts a shadow shares this one array texture,
// indexed by Light.shadowMapLayer -- see ShadowMapArray/ShadowPassManager.
uniform sampler2DArrayShadow shadowMapArray;

// Every point light that casts a shadow shares this one cubemap array, indexed by
// Light.shadowCubeLayer and sampled by direction. Plain (non-shadow) sampler: point
// shadows here store linear distance in the depth texture and compare it manually
// (see calculatePointShadow), since a cubemap's non-linear per-face depth can't be
// reconstructed from a single world-space fragment position the way a single
// lightSpaceMatrix can for directional/spot.
uniform samplerCubeArray pointShadowMapArray;

//uniform samplerCube skybox;
//uniform bool hasSkybox;

float calculateShadow(int layer, mat4 lightSpaceMatrix, vec3 fragPos, vec3 normal, vec3 lightDir);
float calculatePointShadow(int cubeLayer, vec3 lightPos, float farPlane, vec3 fragPos);
vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 materialDiffuse, vec3 materialSpecular);
vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular);
vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular);

void main()
{
    vec3 norm = normalize(outNorm);
    if (material.hasNormalMap) {
        vec3 tangentNormal = texture(material.normal0, outTex).rgb * 2.0 - 1.0;
        norm = normalize(outTBN * tangentNormal);
    }
    vec3 viewDir = normalize(viewPos - outPos);

    vec3 materialDiffuse = material.hasDiffuseMap ? texture(material.diffuse0, outTex).rgb : material.diffuse;
    vec3 materialSpecular = material.hasSpecularMap ? texture(material.specular0, outTex).rgb : material.specular;
    vec3 result = materialDiffuse * ambientLight;

    for (int i = 0; i < numLights; i++) {
        switch (lights[i].type) {
            case 0:
                result += calculateDirectionalLight(lights[i], norm, viewDir, outPos, materialDiffuse, materialSpecular);
                break;
            case 1:
                result += calculatePointLight(lights[i], norm, outPos, viewDir, materialDiffuse, materialSpecular);
                break;
            case 2:
                result += calculateSpotLight(lights[i], norm, outPos, viewDir, materialDiffuse, materialSpecular);
        }
    }

    FragColor = vec4(result, 1.0);

}

float calculateShadow(int layer, mat4 lightSpaceMatrix, vec3 fragPos, vec3 normal, vec3 lightDir) {
    if (layer < 0) return 1.0; // this light has no shadow map this frame -> fully lit

    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // NDC [-1,1] -> [0,1] to match the depth texture

    if (projCoords.z > 1.0) return 1.0; // beyond the shadow frustum's far plane -> treat as lit

    float bias = max(0.0025 * (1.0 - dot(normal, lightDir)), 0.0005);
    // texture() on a sampler2DArrayShadow takes (u, v, layer, compareRef) and does the depth
    // compare (+ free 2x2 hardware PCF), returning 1.0 = fully lit, 0.0 = fully in shadow.
    return texture(shadowMapArray, vec4(projCoords.xy, float(layer), projCoords.z - bias));
}

float calculatePointShadow(int cubeLayer, vec3 lightPos, float farPlane, vec3 fragPos) {
    if (cubeLayer < 0) return 1.0; // this light has no shadow map this frame -> fully lit

    vec3 fragToLight = fragPos - lightPos;
    float currentDistance = length(fragToLight);
    // samplerCubeArray's texture() takes (direction.xyz, arrayLayer) -- direction alone
    // picks the face, so no lightSpaceMatrix is needed the way directional/spot need one.
    float closestDistance = texture(pointShadowMapArray, vec4(fragToLight, float(cubeLayer))).r * farPlane;

    float bias = 0.05;
    return currentDistance - bias > closestDistance ? 0.0 : 1.0;
}

vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 materialDiffuse, vec3 materialSpecular) {
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    // Blinn-Phong specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Phong specular
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 diffuse = light.color * diff * materialDiffuse * light.intensity;
    vec3 specular = light.color * spec * materialSpecular * light.intensity;

    float shadow = calculateShadow(light.shadowMapLayer, light.lightSpaceMatrix, fragPos, normal, lightDir);

    return shadow * (diffuse + specular);
}

vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    // Phong specular
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Blinn-Phong specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuse = light.color * diff * materialDiffuse * light.intensity;
    vec3 specular = light.color * spec * materialSpecular * light.intensity;

    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = calculatePointShadow(light.shadowCubeLayer, light.position, light.shadowFarPlane, fragPos);

    return shadow * (diffuse + specular);
}

vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    // Phong specular
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Blinn-Phong specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 diffuse = light.color * diff * materialDiffuse * light.intensity;
    vec3 specular = light.color * spec * materialSpecular * light.intensity;

    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    float shadow = calculateShadow(light.shadowMapLayer, light.lightSpaceMatrix, fragPos, normal, lightDir);

    return shadow * (diffuse + specular);
}
