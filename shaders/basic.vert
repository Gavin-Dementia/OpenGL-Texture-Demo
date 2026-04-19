#version 330 core

// =======================
// Vertex Attributes
// =======================
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// Instance matrix
layout(location = 3) in vec4 iRow0;
layout(location = 4) in vec4 iRow1;
layout(location = 5) in vec4 iRow2;
layout(location = 6) in vec4 iRow3;

// =======================
// UBO: Camera
// =======================
layout(std140) uniform Camera {
    mat4 view;
    mat4 projection;
    vec4 viewPos; // padding
};

// =======================
// Outputs
// =======================
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    mat4 modelMat = mat4(iRow0, iRow1, iRow2, iRow3);

    vec4 worldPos = modelMat * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;

    Normal = mat3(transpose(inverse(modelMat))) * aNormal;

    gl_Position = projection * view * worldPos;

    TexCoords = aTexCoord;
}

