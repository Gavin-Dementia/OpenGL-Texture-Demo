#version 430 core

layout(location = 0) in vec3 aPos;

// per-instance draw command visualization
layout(std430, binding = 5) buffer IndirectCommands
{
    uint commands[];
};

uniform mat4 view;
uniform mat4 projection;

flat out vec3 debugColor;

void main()
{
    uint cmdIndex = gl_InstanceID;

    uint base = cmdIndex * 5;

    uint indexCount   = commands[base + 0];
    uint instanceCnt  = commands[base + 1];
    uint firstIndex   = commands[base + 2];
    uint baseVertex   = commands[base + 3];
    uint baseInstance = commands[base + 4];

    // encode debug color
    debugColor = vec3(
        float(instanceCnt) * 0.01,
        float(indexCount) * 0.001,
        float(baseVertex) * 0.001
    );

    vec4 pos = vec4(aPos + vec3(cmdIndex % 10, cmdIndex / 10, 0), 1.0);

    gl_Position = projection * view * pos;
}

