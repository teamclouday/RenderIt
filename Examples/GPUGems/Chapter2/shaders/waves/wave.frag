#version 450 core

layout(location = 0) out vec4 outColor;

layout(location = 0) in VERTOUT
{
    vec3 normWS;
    vec4 posWS;
}
vertOut;

uniform vec3 surfColor;
uniform float surfAlpha;
uniform vec3 cameraPosWS;

// stage 5
void main()
{
    vec3 normDir = normalize(vertOut.normWS);
    vec3 viewDir = normalize(cameraPosWS - vertOut.posWS.xyz);
    const vec3 lightDir = vec3(0.0, 1.0, 0.0);

    float intensity = dot(normDir, lightDir);
    float diff = max(0.0, intensity) * 1.5;
    outColor = vec4(diff * surfColor, surfAlpha);
}
