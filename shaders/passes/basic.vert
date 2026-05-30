#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// =========================
// GPU compacted visibility list
layout(std430, binding = 8) buffer VisibilityBuffer
{
    uint instanceIndices[];
};

// =========================
// Core buffers
struct Instance
{
    uint transformID;
    uint meshID;
    uint materialID;
    uint visibilityID;
};

layout(std430, binding = 3) buffer InstanceBuffer
{
    Instance instances[];
};

layout(std430, binding = 1) buffer TransformBuffer
{
    mat4 transforms[];
};

layout(std430, binding = 7) buffer DebugBuffer
{
    vec4 debugColor[];
};

// =========================
// Camera
layout(std140, binding = 0) uniform CameraUBO
{
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

// =========================
// Outputs
flat out uint vInstanceID;
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 vDebugColor;

// =========================
// MAIN
void main()
{
    uint drawID = gl_InstanceID;

    uint instanceID = instanceIndices[drawID];

    vInstanceID = instanceID;

    Instance inst = instances[instanceID];
    mat4 model = transforms[inst.transformID];

    vec4 worldPos = model * vec4(aPos, 1.0);

    FragPos = worldPos.xyz;
    TexCoords = aTexCoord;

    Normal = mat3(transpose(inverse(model))) * aNormal;

    //gl_Position = projection * view * worldPos;
gl_Position = vec4(aPos, 1.0);
}

