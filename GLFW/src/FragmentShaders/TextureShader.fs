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

uniform Material material;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
    vec3 ambient;
    if (material.hasDiffuseMap) {
        ambient = lightColor * texture(material.diffuse0, outTex).rgb * material.ambient;
    } else {
        ambient = lightColor * material.diffuse * material.ambient;
    }

    vec3 diffuse;
    vec3 norm = normalize(outNorm);
    vec3 lightDir = normalize(lightPos - outPos);
    float diff = max(dot(norm, lightDir), 0.0);

    if (material.hasDiffuseMap) {
        diffuse = lightColor * diff * texture(material.diffuse0, outTex).rgb;
    } else {
        diffuse = lightColor * diff * material.diffuse;
    }

    vec3 specular;
    vec3 viewDir = normalize (viewPos - outPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    if (material.hasSpecularMap) {
        specular = lightColor * spec * texture(material.specular0, outTex).rgb;
    } else {
        specular = lightColor * spec * material.specular;
    }
    vec3 result = (ambient + diffuse + specular) * 2.0;
    FragColor = vec4(result, 1.0);
}