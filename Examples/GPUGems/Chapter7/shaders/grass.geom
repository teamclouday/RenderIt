#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 9) out;

layout(location = 0) in vec2 vertTex[];
layout(location = 1) in int vertID[];
layout(location = 0) out vec2 baseUV;

uniform mat4 matProjView;
uniform float valTime;
uniform vec3 vecWind;
uniform float valWind;

layout(std430, binding = 1) readonly buffer GrassData
{
    vec2 grassOffsets[];
};

#define SQRT3_2 0.8660254037844386

const vec2 grassRotations[3] =
    vec2[3](vec2(0.0, 0.0), vec2(-0.1 * 0.5, SQRT3_2 * 0.1), vec2(-0.1 * 1.5, SQRT3_2 * 0.1));

vec3 ComputeTranslation(vec2 objectCenter, float texY)
{
    vec2 rnd = objectCenter + 1.0;
    vec3 offsets = vec3(0.0);
    offsets.x = clamp(0.05 * (valWind * vecWind.x + sin((valTime + rnd.x) * rnd.y)), -0.1, 0.1);
    offsets.y = clamp(0.01 * (valWind * vecWind.y + sin(valTime)), -0.05, 0.05);
    offsets.z = clamp(0.05 * (valWind * vecWind.z + cos((valTime + rnd.x) * rnd.y)), -0.1, 0.1);
    return mix(vec3(0.0), offsets, texY);
}

void main()
{
    for (int rot = 0; rot < 3; ++rot)
    {
        vec2 xz = grassRotations[rot];
        for (int i = 0; i < 3; ++i)
        {
            baseUV = vertTex[i];
            vec4 pos = gl_in[i].gl_Position;
            vec2 center = 1.8 * grassOffsets[vertID[i]];
            pos.xz += sign(pos.x) * xz + center;
            pos.xyz += ComputeTranslation(center, baseUV.y);
            gl_Position = matProjView * pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
