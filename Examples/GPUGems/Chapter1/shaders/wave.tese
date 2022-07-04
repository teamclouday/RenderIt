#version 450 core

layout(triangles, equal_spacing, ccw) in;

// stage 3
void main()
{
    // clang-format off
    gl_Position = gl_TessCoord.x * gl_in[0].gl_Position +
                  gl_TessCoord.y * gl_in[1].gl_Position +
                  gl_TessCoord.z * gl_in[2].gl_Position;
    // clang-format on
}
