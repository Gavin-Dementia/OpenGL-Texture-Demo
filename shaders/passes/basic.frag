#version 430 core

// =========================
// Inputs

flat in uint vInstanceID;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 vDebugColor;

out vec4 FragColor;

// =========================
// Data structures

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
    vec4 params;
};

// =========================
// SSBOs

layout(std430, binding = 3) buffer InstanceBuffer
{    Instance instances[];  };

layout(std430, binding = 5) buffer MaterialBuffer
{    Material materials[];  };

layout(std430, binding = 9) buffer DirLightBuffer
{    DirLightGPU dirLight;  };

layout(std430, binding = 8) buffer VisibilityBuffer
{    uint instanceIndices[];  };

layout(std430, binding = 10) buffer PointLightBuffer
{    PointLightGPU pointLights[];  };

// =========================
// Camera

layout(std140, binding = 0) uniform CameraUBO
{
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

// =========================
// Textures

layout(binding = 0) uniform sampler2D textures[32];

// =========================
// Lighting

vec3 CalcDirLight(vec3 N, vec3 V, vec3 diffuse, vec3 specular, Material mat)
{
    vec3 L = normalize(-dirLight.direction.xyz);

    float diff = max(dot(N, L), 0.0);
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), mat.shininess);

    return dirLight.ambient.xyz * diffuse +
           dirLight.diffuse.xyz * diff * diffuse +
           dirLight.specular.xyz * spec * specular;
}

vec3 CalcPointLight(PointLightGPU light, vec3 N, vec3 P, vec3 V, vec3 diffuse, vec3 specular, Material mat)
{
    vec3 L = normalize(light.position.xyz - P);

    float diff = max(dot(N, L), 0.0);
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), mat.shininess);

    float d = length(light.position.xyz - P);

    float att = 1.0 / (
        light.params.x +
        light.params.y * d +
        light.params.z * d * d
    );

    vec3 result =
        light.ambient.xyz * diffuse +
        light.diffuse.xyz * diff * diffuse +
        light.specular.xyz * spec * specular;

    return result * att;
}

void main()
{
    Instance inst = instances[vInstanceID];
    Material mat = materials[inst.materialID];

    vec3 diffuseTex = texture(textures[mat.diffuseTex], TexCoords).rgb;
    vec3 specularTex = texture(textures[mat.specularTex], TexCoords).rgb;

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos.xyz - FragPos);

    vec3 color = vec3(0.0);

    color += CalcDirLight(N, V, diffuseTex, specularTex, mat);

    for (int i = 0; i < 16; i++)
    {
        if (pointLights[i].params.w <= 0.0) continue;

        color += CalcPointLight(pointLights[i],
                                N, FragPos, V,
                                diffuseTex, specularTex, mat);
    }

    color += mat.emissiveColor.xyz;

    FragColor = vec4(color, 1.0);
//FragColor = vec4(1,0,0,1);
}

