#version 330 core
out vec4 FragColor;
in vec3 outPos;
in vec3 outNorm;
in vec2 outTex;

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
};

uniform Material material;
uniform Light[16] lights;

uniform vec3 ambientLight;
uniform int numLights;
uniform vec3 viewPos;

//uniform samplerCube skybox;
//uniform bool hasSkybox;

vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular);
vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular);
vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular);

void main()
{
    vec3 norm = normalize(outNorm);
    vec3 viewDir = normalize(viewPos - outPos);

    vec3 materialDiffuse = material.hasDiffuseMap ? texture(material.diffuse0, outTex).rgb : material.diffuse;
    vec3 materialSpecular = material.hasSpecularMap ? texture(material.specular0, outTex).rgb : material.specular;
    vec3 result = material.ambient * ambientLight;

    for (int i = 0; i < numLights; i++) {
        switch (lights[i].type) {
            case 0:
                result += calculateDirectionalLight(lights[i], norm, viewDir, materialDiffuse, materialSpecular);
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

vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 diffuse = light.color * diff * materialDiffuse * light.intensity;
    vec3 specular = light.color * spec * materialSpecular * light.intensity;
    return  (diffuse + specular);
}

vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuse = light.color * diff * materialDiffuse * light.intensity;
    vec3 specular = light.color * spec * materialSpecular * light.intensity;

    diffuse *= attenuation;
    specular *= attenuation;

    return (diffuse + specular);
}

vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialDiffuse, vec3 materialSpecular) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

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

    return (diffuse + specular);
}