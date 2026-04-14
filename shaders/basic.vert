#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    // world position
    vec4 worldPos = modelMat * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;

    // correct normal transform (non-uniform scaling safe)
    Normal = mat3(transpose(inverse(modelMat))) * aNormal;

    // clip space position
    gl_Position = projMat * viewMat * worldPos;

    TexCoords = aTexCoord;
}