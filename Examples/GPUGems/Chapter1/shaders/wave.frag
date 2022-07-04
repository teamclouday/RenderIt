#version 450 core

layout(location = 0) out vec4 outColor;

layout(location = 0) in VERTOUT
{
    vec3 normWS;
    vec4 posWS;
}
vertOut;

uniform vec3 surfColor;
uniform vec3 cameraPosWS;
uniform bool displayNormal;

// stage 5
void main()
{
    vec3 normDir = normalize(vertOut.normWS);
    vec3 viewDir = normalize(cameraPosWS - vertOut.posWS.xyz);
    const vec3 lightDir = normalize(vec3(1.0));
    if (displayNormal)
    {
        outColor = vec4(normDir * 0.5 + 0.5, 1.0);
    }
    else
    {
        float intensity = dot(normDir, lightDir);
        float diff = max(0.0, intensity) * 1.5;
        outColor = vec4(diff * surfColor, 1.0);
    }
}
