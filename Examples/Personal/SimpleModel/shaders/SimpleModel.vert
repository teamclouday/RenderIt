#version 450 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBiTangent;

layout(location = 0) out VERTOUT
{
    vec3 normalWS;
    vec3 tangentWS;
    vec3 bitangentWS;
    vec2 texCoords;
    vec4 fragPosWS;
}
vertOut;

uniform mat4 mat_ProjView;
uniform mat4 mat_model;
uniform mat3 mat_modelInv;

void main()
{
    vertOut.normalWS = normalize(inNormal * mat_modelInv);
    vertOut.tangentWS = normalize(inTangent * mat_modelInv);
    vertOut.bitangentWS = normalize(inBiTangent * mat_modelInv);
    vertOut.texCoords = inTexCoords;
    vertOut.fragPosWS = mat_model * vec4(inPos, 1.0);
    gl_Position = mat_ProjView * vertOut.fragPosWS;
}
