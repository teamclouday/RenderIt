#version 450 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out VERTDATA
{
    vec3 normal;
    vec2 texCoords;
}
vertOut;

void main()
{
    vertOut.normal = inNormal;
    vertOut.texCoords = inTexCoords;
    gl_Position = vec4(inPos, 1.0);
}
