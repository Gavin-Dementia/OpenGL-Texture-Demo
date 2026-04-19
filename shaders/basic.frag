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
// =======================
// Point Lights -> UBO
// =======================
#define NR_POINT_LIGHTS 16

struct PointLightGPU {
    vec4 position;    // xyz

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec4 params;      // x=constant, y=linear, z=quadratic, w=intensity
};

layout(std140) uniform PointLightsBlock {
    PointLightGPU pointLights[NR_POINT_LIGHTS];
};

// =======================
// SpotLight -> UBO
// =======================
struct SpotLightGPU {
    vec4 position;
    vec4 direction;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec4 params1; // x=cutOff y=outerCutOff z=constant w=linear
    vec4 params2; // x=quadratic y=intensity
};

layout(std140) uniform SpotLightBlock {
    SpotLightGPU spotLight;
};

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

vec3 CalcPointLight(PointLightGPU light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseTex, vec3 specTex)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position.xyz - fragPos);
    float constant = light.params.x;
    float linear = light.params.y;
    float quadratic = light.params.z;
    float attenuation = 1.0 / (
        constant +
        linear * distance +
        quadratic * distance * distance
    );

    vec3 ambientC = light.ambient.xyz * diffuseTex;
    vec3 diffuseC = light.diffuse.xyz * diff * diffuseTex;
    vec3 specularC = light.specular.xyz * spec * specTex;

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

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        if(pointLights[i].params.w <= 0.0) continue; // intensity == 0 → skip
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, diffuseTex, specTex);
    }
    
    if (isEmissive)
        result += emissiveColor;

    FragColor = vec4(result, 1.0);
}

