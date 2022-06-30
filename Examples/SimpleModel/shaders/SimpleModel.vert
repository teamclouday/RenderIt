#version 450 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in vec4 inColor;

layout (location = 0) out vec2 outTexCoords;

uniform mat4 mvp;

void main()
{
    outTexCoords = inTexCoords;
    gl_Position = mvp * vec4(inPos, 1.0);
}
