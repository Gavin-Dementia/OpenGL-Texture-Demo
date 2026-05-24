#version 430 core

in vec3 debugColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(debugColor, 1.0);
}

