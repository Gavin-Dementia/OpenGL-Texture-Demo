#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

struct Instance
{
    uint transformID;
    uint meshID;
    uint materialID;
    uint visibilityID;
};

layout(std430, binding = 1) buffer TransformBuffer
{
    mat4 transforms[];
};

layout(std430, binding = 3) buffer InstanceBuffer
{
    Instance instances[];
};

layout(std140, binding = 0) uniform CameraUBO
{
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

out VS_OUT
{
    flat uint instanceID;
    vec3 worldPos;
    vec3 normal;
    vec2 uv;
} vs_out;
#if 0
void main()
{
    uint instanceID = gl_InstanceID;

    Instance inst = instances[instanceID];
    mat4 model = transforms[inst.transformID];

    vec4 wp = model * vec4(pos, 1.0);

    vs_out.instanceID = instanceID;
    vs_out.worldPos = wp.xyz;
    vs_out.uv = uv;
    vs_out.normal = mat3(transpose(inverse(model))) * normal;

    //gl_Position = projection * view * wp;
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}
#else
void main()
{
    mat4 vp = projection * view;
    gl_Position = vp * vec4(pos, 1.0);
}
#endif

