#version 430 core
layout (location = 0) in vec3 aPos;

// Use the same Camera UBO as other shaders (binding = 0)
layout(std140) uniform Camera {
    mat4 view;
    mat4 projection;
    vec4 viewPos; // padding / camera position
};

uniform mat4 modelLight;

void main()
{
    gl_Position = projection * view * modelLight * vec4(aPos, 1.0);
}

