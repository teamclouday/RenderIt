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

uniform mat4 mat_VP;
uniform mat4 mat_model;
uniform mat4 mat_modelInv;

void main()
{
    vertOut.normalWS = normalize((vec4(inNormal, 0.0) * mat_modelInv).xyz);
    vertOut.tangentWS = normalize(mat3(mat_model) * inTangent);
    vertOut.bitangentWS = normalize(mat3(mat_model) * inBiTangent);
    vertOut.texCoords = inTexCoords;
    vertOut.fragPosWS = mat_model * vec4(inPos, 1.0);
    gl_Position = mat_VP * vertOut.fragPosWS;
}
