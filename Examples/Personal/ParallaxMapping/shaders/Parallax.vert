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
    vec3 viewTS;
}
vertOut;

uniform mat4 mat_ProjView;
uniform mat4 mat_Model;
uniform mat3 mat_ModelInv;
uniform vec3 vec_CameraPosWS;

void main()
{
    vertOut.normalWS = normalize(inNormal * mat_ModelInv);
    vertOut.tangentWS = normalize(inTangent * mat_ModelInv);
    vertOut.bitangentWS = normalize(inBiTangent * mat_ModelInv);
    vertOut.texCoords = inTexCoords * 8.0;
    vertOut.fragPosWS = mat_Model * vec4(inPos, 1.0);
    gl_Position = mat_ProjView * vertOut.fragPosWS;

    mat3 TBN = transpose(mat3(vertOut.tangentWS, vertOut.bitangentWS, vertOut.normalWS));
    vertOut.viewTS = normalize(TBN * (vec_CameraPosWS - vertOut.fragPosWS.xyz));
}
