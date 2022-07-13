#version 450 core

layout(location = 0) out vec4 outColor;
layout(location = 0) in TESSEDATA
{
    vec3 normal;
    float height;
    vec4 fragPos;
}
tesseOut;

uniform vec3 baseColor;
uniform vec3 cameraPos;

void main()
{
    const vec3 lightDir = normalize(vec3(1.0));
    vec3 normDir = normalize(tesseOut.normal);
    vec3 viewDir = normalize(cameraPos - tesseOut.fragPos.xyz);
    float diff = max(dot(normDir, lightDir), 0.0);
    float spec = pow(max(dot(reflect(-lightDir, normDir), viewDir), 0.0), 32.0);
    outColor = vec4(baseColor * (diff + spec), 1.0);
}
