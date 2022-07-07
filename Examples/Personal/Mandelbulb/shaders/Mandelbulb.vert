#version 450 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inCoords;

layout(location = 0) out vec2 vertCoords;

void main()
{
    vertCoords = inCoords;
    gl_Position = vec4(inPos, 0.0, 1.0);
}
