#version 450 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out VERTOUT
{
    vec4 fragPosWS;
    vec2 texCoords;
}
vertOut;

uniform mat4 mat_ProjView;
uniform mat4 mat_Model;

void main()
{
    vertOut.fragPosWS = mat_Model * vec4(inPos, 1.0);
    vertOut.texCoords = inTexCoords;
    gl_Position = mat_ProjView * vertOut.fragPosWS;
}
