#version 450 core

layout(location = 0) out vec4 outColor;
layout(location = 0) in VERTOUT
{
    vec3 normalWS;
    vec4 fragPosWS;
}
vertOut;

void main()
{
    outColor = vec4(1.0, 0.5, 0.2, 1.0);
}
