#version 330 core
// =======================
// Vertex Attributes (mesh)
// =======================
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// =======================
// Instance Matrix (mat4 = 4 vec4 attributes)
// =======================
layout(location = 3) in vec4 iRow0;
layout(location = 4) in vec4 iRow1;
layout(location = 5) in vec4 iRow2;
layout(location = 6) in vec4 iRow3;

// =======================
// Outputs
// =======================
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

// =======================
// Camera
// =======================
uniform mat4 viewMat;
uniform mat4 projMat;

// =======================
// Main
// =======================
void main()
{
    // rebuild instance matrix
    mat4 modelMat = mat4(iRow0, iRow1, iRow2, iRow3);

    // world position
    vec4 worldPos = modelMat * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;

    // correct normal transform (non-uniform scaling safe)
    Normal = mat3(transpose(inverse(modelMat))) * aNormal;

    // clip space position
    gl_Position = projMat * viewMat * worldPos;

    TexCoords = aTexCoord;
}

