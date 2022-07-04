#version 450 core

layout(location = 0) in vec2 inPosition;

// stage 1
void main()
{
    gl_Position = vec4(inPosition, 0.0, 1.0);
}
