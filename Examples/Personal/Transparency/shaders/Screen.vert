#version 450 core
layout(location = 0) in vec2 inPos;
layout(location = 0) out vec2 vertTexCoords;

void main()
{
    vertTexCoords = inPos * 0.5 + 0.5;
    gl_Position = vec4(inPos, 0.0, 1.0);
}
