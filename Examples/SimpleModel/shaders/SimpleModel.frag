#version 450 core

layout (location = 0) out vec4 outColor;
layout (location = 0) in vec2 inTexCoords;
layout (location = 1) in vec3 inNormal;

uniform sampler2D map_DIFFUSE;
uniform bool map_DIFFUSE_exists;

void main()
{
    if(map_DIFFUSE_exists)
    {
        outColor = texture(map_DIFFUSE, inTexCoords);
    }
    else
    {
        outColor = vec4(inNormal, 1.0);
    }
}
