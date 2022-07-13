#version 450 core

layout(vertices = 3) out;

layout(location = 0) in VERTDATA
{
    vec3 normal;
    vec2 texCoords;
}
vertOut[];

layout(location = 0) out TESSCDATA
{
    vec3 normal;
    vec2 texCoords;
}
tesscOut[];

uniform float tessLevel;

// stage 2
void main()
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = tessLevel;
        // gl_TessLevelInner[1] = tessLevel;

        gl_TessLevelOuter[0] = tessLevel;
        gl_TessLevelOuter[1] = tessLevel;
        gl_TessLevelOuter[2] = tessLevel;
        // gl_TessLevelOuter[3] = tessLevel;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tesscOut[gl_InvocationID].normal = vertOut[gl_InvocationID].normal;
    tesscOut[gl_InvocationID].texCoords = vertOut[gl_InvocationID].texCoords;
}
