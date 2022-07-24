#version 450 core

layout(location = 0) out vec2 vertTex;
layout(location = 1) out int vertID;

const vec2 positions[4] = vec2[4](vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0));

void main()
{
    vertID = gl_InstanceID;
    vec2 pos = positions[gl_VertexID];
    vertTex = pos * 0.5 + 0.5;
    vertTex.y = -vertTex.y;
    gl_Position = vec4(pos.x * 0.1, pos.y * 0.1 + 0.1, 0.0, 1.0);
}
