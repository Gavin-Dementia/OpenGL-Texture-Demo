#version 330 core
layout(location = 0) in vec3 aPos; //in cube.h
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 FragPos;
out vec3 Normal;
out vec2 Texcoord;

void main()
{
    FragPos = vec3(modelMat * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(modelMat))) * aNormal;  
    gl_Position = projMat * viewMat * vec4(FragPos, 1.0);
    Texcoord= aTexcoord;
}