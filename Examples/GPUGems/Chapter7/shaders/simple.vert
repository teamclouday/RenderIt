#version 450 core

layout(location = 0) in vec3 inPos;

uniform mat4 matMVP;

void main()
{
    gl_Position = matMVP * vec4(inPos, 1.0);
}
