#version 450 core

layout(location = 0) out vec4 outColor;

uniform sampler2DMS screenTexture;
uniform int numSamples;

void main()
{
    ivec2 texCoords = ivec2(gl_FragCoord.xy);
    vec3 color = vec3(0.0);
    for (int i = 0; i < numSamples; ++i)
    {
        color += texelFetch(screenTexture, texCoords, i).rgb;
    }
    color /= numSamples;
    outColor = vec4(color, 1.0);
}
