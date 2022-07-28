#version 450 core

#define MAX_VERTICES 180 // not including center
#define PI 3.14159265359

layout(points) in;
layout(triangle_strip, max_vertices = MAX_VERTICES) out;

layout(location = 0) out GEOMOUT
{
    vec3 normalWS;
    vec3 tangentWS;
    vec4 fragPosWS;
}
geomOut;

uniform float count;
uniform float countInv;
uniform float radius;
uniform mat4 matProjView;
uniform mat4 matModel;
uniform mat3 matModelInv;

void main()
{
    vec4 center = gl_in[0].gl_Position;
    float d = 2.0 * PI * countInv;
    for (int i = 0; i < count; ++i)
    {
        // V1
        gl_Position = matProjView * center;
        geomOut.fragPosWS = center;
        geomOut.normalWS = normalize(matModelInv * vec3(0.0, 1.0, 0.0));
        geomOut.tangentWS = vec3(0.0, 0.0, 0.0);
        EmitVertex();
        float theta = i * d;
        vec2 off1 = radius * vec2(cos(theta), sin(theta));
        vec2 off2 = radius * vec2(cos(theta - d), sin(theta - d));

        // V2
        vec4 pos = center;
        pos.xz += off1;
        geomOut.fragPosWS = matModel * pos;
        gl_Position = matProjView * geomOut.fragPosWS;
        geomOut.normalWS = normalize(matModelInv * vec3(0.0, 1.0, 0.0));
        geomOut.tangentWS = normalize(matModelInv * cross(geomOut.normalWS, geomOut.fragPosWS.xyz));
        EmitVertex();

        // V3
        pos = center;
        pos.xz += off2;
        geomOut.fragPosWS = matModel * pos;
        gl_Position = matProjView * geomOut.fragPosWS;
        geomOut.normalWS = normalize(matModelInv * vec3(0.0, 1.0, 0.0));
        geomOut.tangentWS = normalize(matModelInv * cross(geomOut.normalWS, geomOut.fragPosWS.xyz));
        EmitVertex();

        EndPrimitive();
    }
}
