#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

//uniform float brightness;

out vec4 vertexColor;
out vec2 TexCoord;

void main()
{
    gl_Position = projMat * viewMat * modelMat * vec4(aPos, 1.0); 
    //vertexColor = vec4(aColor.rgb * brightness, aColor.a);
    TexCoord= vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}

