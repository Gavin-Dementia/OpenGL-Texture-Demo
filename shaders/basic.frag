#version 330 core
in vec2 TexCoord;
in vec4 vertexColor;

uniform sampler2D ourTextureW;
uniform sampler2D ourTextureF;
//uniform vec4 ourColor;

out vec4 FragColor;

void main()
{
    vec4 tex1 = texture(ourTextureW, TexCoord);
    vec4 tex2 = texture(ourTextureF, TexCoord);

    vec4 texColor = mix(tex1, tex2, 0.5);

    // 避免過曝（可選）
    FragColor = texColor * vertexColor;
}

