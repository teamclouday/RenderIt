#version 450 core
layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 vertTexCoords;

uniform sampler2D screenTexture;

void main()
{
    outColor = vec4(texture(screenTexture, vertTexCoords).rgb, 1.0);
}
