#version 430 core

in VS_OUT
{
    flat uint instanceID;
    vec3 worldPos;
    vec3 normal;
    vec2 uv;
} fs_in;

out vec4 FragColor;

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

layout(std430, binding = 3) buffer InstanceBuffer
{
    Instance instances[];
};

layout(std430, binding = 5) buffer MaterialBuffer
{
    Material materials[];
};

layout(std140, binding = 0) uniform CameraUBO
{
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

layout(binding = 0) uniform sampler2D textures[32];

void main()
{
    Instance inst = instances[fs_in.instanceID];
    Material mat = materials[inst.materialID];

    vec3 albedo = texture(textures[mat.diffuseTex], fs_in.uv).rgb;
    vec3 spec = texture(textures[mat.specularTex], fs_in.uv).rgb;

    vec3 N = normalize(fs_in.normal);
    vec3 V = normalize(viewPos.xyz - fs_in.worldPos);

    vec3 color = albedo * mat.albedoColor.rgb;
    color += mat.emissiveColor.rgb;

    //FragColor = vec4(color, 1.0);
    FragColor = vec4(1,0,0,1);
}

