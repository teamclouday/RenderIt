#version 450 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in vec4 inColor;

layout (location = 0) out VERTOUT
{
    vec3 normalWS;
    vec2 texCoords;
    vec4 color;
    vec4 fragPosWS;
} vertOut;


uniform mat4 mat_VP;
uniform mat4 mat_model;
uniform mat4 mat_modelInv;

void main()
{
    vertOut.normalWS = normalize((vec4(inNormal, 0.0) * mat_modelInv).xyz);
    vertOut.texCoords = inTexCoords;
    vertOut.color = inColor;
    vertOut.fragPosWS = mat_model * vec4(inPos, 1.0);
    gl_Position = mat_VP * vertOut.fragPosWS;
}
