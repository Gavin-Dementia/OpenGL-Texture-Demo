#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// =======================
// UBO: Camera
// =======================
layout(std140) uniform Camera {
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

// =======================
// UBO: DirLight
// =======================
layout(std140) uniform DirLightBlock {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

// =======================
// Material
// =======================
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;

// =======================
// Point Light（先不進 UBO）
// =======================
#define NR_POINT_LIGHTS 16

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform int numPointLights;
uniform PointLight pointLights[NR_POINT_LIGHTS];

// =======================
// Emissive
// =======================
uniform bool isEmissive;
uniform vec3 emissiveColor;

// =======================
// Functions
// =======================
vec3 CalcDirLight(vec3 normal, vec3 viewDir, vec3 diffuseTex, vec3 specTex)
{
    vec3 lightDir = normalize(-direction.xyz);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambientC = ambient.xyz * diffuseTex;
    vec3 diffuseC = diffuse.xyz * diff * diffuseTex;
    vec3 specularC = specular.xyz * spec * specTex;

    return ambientC + diffuseC + specularC;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseTex, vec3 specTex)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (
        light.constant +
        light.linear * distance +
        light.quadratic * distance * distance
    );

    vec3 ambientC = light.ambient * diffuseTex;
    vec3 diffuseC = light.diffuse * diff * diffuseTex;
    vec3 specularC = light.specular * spec * specTex;

    ambientC *= attenuation;
    diffuseC *= attenuation;
    specularC *= attenuation;

    return ambientC + diffuseC + specularC;
}

void main()
{
    vec3 diffuseTex = vec3(texture(material.diffuse, TexCoords));
    vec3 specTex = vec3(texture(material.specular, TexCoords));

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos.xyz - FragPos);

    vec3 result = vec3(0.0);

    result += CalcDirLight(norm, viewDir, diffuseTex, specTex);

    for(int i = 0; i < numPointLights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, diffuseTex, specTex);

    if (isEmissive)
        result += emissiveColor;

    FragColor = vec4(result, 1.0);
}

