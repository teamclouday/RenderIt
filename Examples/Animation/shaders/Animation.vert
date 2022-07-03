#version 450 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBiTangent;
layout(location = 5) in uvec4 inBoneIDs;
layout(location = 6) in vec4 inBoneWeights;

layout(location = 0) out VERTOUT
{
    vec3 normalWS;
    vec3 tangentWS;
    vec3 bitangentWS;
    vec2 texCoords;
    vec4 fragPosWS;
}
vertOut;

#define MAX_NUM_BONES 300

layout(std140, binding = 0) uniform BoneMatrices
{
    mat4 boneMats[MAX_NUM_BONES];
};

uniform mat4 mat_VP;
uniform mat4 mat_model;
uniform mat3 mat_modelInv;

void main()
{
    mat4 boneTransform;
    mat3 normInvMat;

    if ((inBoneWeights[0] + inBoneWeights[1] + inBoneWeights[2] + inBoneWeights[3]) != 0.0)
    {
        boneTransform = boneMats[inBoneIDs[0]] * inBoneWeights[0];
        boneTransform += boneMats[inBoneIDs[1]] * inBoneWeights[1];
        boneTransform += boneMats[inBoneIDs[2]] * inBoneWeights[2];
        boneTransform += boneMats[inBoneIDs[3]] * inBoneWeights[3];
        normInvMat = inverse(mat3(boneTransform));
    }
    else
    {
        boneTransform = mat4(1.0);
        normInvMat = mat3(1.0);
    }

    vertOut.normalWS = normalize(inNormal * normInvMat * mat_modelInv);
    vertOut.tangentWS = normalize(inTangent * normInvMat * mat_modelInv);
    vertOut.bitangentWS = normalize(inBiTangent * normInvMat * mat_modelInv);
    vertOut.texCoords = inTexCoords;
    vertOut.fragPosWS = mat_model * boneTransform * vec4(inPos, 1.0);
    gl_Position = mat_VP * vertOut.fragPosWS;
}
