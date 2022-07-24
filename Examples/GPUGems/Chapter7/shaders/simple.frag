#version 450 core

layout(location = 0) out vec4 outColor;

uniform vec3 val_DIFFUSE;

void main()
{
    outColor = vec4(val_DIFFUSE, 1.0);
}
