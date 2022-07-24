#version 450 core

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 baseUV;

uniform sampler2D grassTexture;

void main()
{
    vec4 color = texture(grassTexture, baseUV);
    if (color.a < 1.0)
    {
        discard;
    }
    else
    {
        outColor = color;
    }
}
