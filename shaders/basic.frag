#version 430 core

// =========================
// Inputs from vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 vDebugColor;

// instance index (from indirect draw / vertex shader passthrough)
flat in uint instanceID;

// =========================
// Output
out vec4 FragColor;

// =========================
// GPU Scene Buffers

struct Instance
{
    uint transformID;
    uint meshID;
    uint materialID;
    uint visibilityID;
};

struct Material
{
    vec4 albedoColor;
    vec4 emissiveColor;

    uint diffuseTex;
    uint specularTex;
    float shininess;
    uint flags;
};

struct DirLightGPU
{
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct PointLightGPU
{
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 params; // x=constant y=linear z=quadratic w=intensity
};

// =========================
// SSBO bindings

layout(std430, binding = 1) buffer Materials
{    Material materials[];    };

layout(std430, binding = 3) buffer Instances
{    Instance instances[];    };

#define NR_POINT_LIGHTS 16

layout(std430, binding = 4) buffer DirLightBuffer
{    DirLightGPU dirLight;    };

layout(std430, binding =5) buffer PointLights
{    PointLightGPU pointLights[];    };

// =========================
// Textures (global bindless-style array)
layout(binding = 0) uniform sampler2D textures[32];

// =========================
// Camera (UBO)
layout(std140, binding = 0) uniform CameraUBO
{
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

// =========================
// Shadow
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

// =========================
// Helpers
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 proj = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj = proj * 0.5 + 0.5;

    if (proj.x < 0.0 || proj.x > 1.0 ||
        proj.y < 0.0 || proj.y > 1.0)
        return 0.0;

    float closestDepth = texture(shadowMap, proj.xy).r;
    float currentDepth = proj.z;

    float bias = max(0.0005, 0.005 * (1.0 - dot(normal, lightDir)));

    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

// =========================
// Lighting
vec3 CalcDirLight(vec3 normal, vec3 viewDir, vec3 diffuse, vec3 specular, Material mat)
{
    vec3 lightDir = normalize(-dirLight.direction.xyz);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);

    float shadow = 0.0; // optional: pass fragPosLightSpace if needed

    vec3 ambient = dirLight.ambient.xyz * diffuse;
    vec3 diffuseC = dirLight.diffuse.xyz * diff * diffuse;
    vec3 specularC = dirLight.specular.xyz * spec * specular;

    return ambient + diffuseC + specularC;
}

vec3 CalcPointLight(PointLightGPU light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuse, vec3 specular, Material mat)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);

    float distance = length(light.position.xyz - fragPos);

    float attenuation = 1.0 / (
        light.params.x +
        light.params.y * distance +
        light.params.z * distance * distance
    );

    vec3 ambient = light.ambient.xyz * diffuse;
    vec3 diffuseC = light.diffuse.xyz * diff * diffuse;
    vec3 specularC = light.specular.xyz * spec * specular;

    return (ambient + diffuseC + specularC) * attenuation;
}

// =========================
// MAIN
void main()
{
    // -------------------------
    // Fetch instance
    Instance inst = instances[instanceID];
    Material mat = materials[inst.materialID];

    // -------------------------
    // Texture fetch
    vec3 diffuseTex = texture(textures[mat.diffuseTex], TexCoords).rgb;
    vec3 specularTex = texture(textures[mat.specularTex], TexCoords).rgb;

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos.xyz - FragPos);

    vec3 result = vec3(0.0);

    // -------------------------
    // Directional light
    result += CalcDirLight(N, V, diffuseTex, specularTex, mat);

    // -------------------------
    // Point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        if (pointLights[i].params.w <= 0.0)
            continue;

        result += CalcPointLight(
            pointLights[i],
            N,
            FragPos,
            V,
            diffuseTex,
            specularTex,
            mat
        );
    }

    // -------------------------
    // Emissive
    result += mat.emissiveColor.xyz;

    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    FragColor = vec4(result+0.5, 1.0);
    //FragColor = vDebugColor;
}

