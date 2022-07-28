#version 450 core

layout(location = 0) out vec4 outColor;
layout(location = 0) in GEOMOUT
{
    vec3 normalWS;
    vec3 tangentWS;
    vec4 fragPosWS;
}
geomOut;

// other uniforms
uniform vec3 camPosWS;
uniform samplerCube skybox;

uniform float r;
uniform float d;
uniform float radius;

vec3 SampleSkybox(vec3 V, vec3 N)
{
    vec3 uvw = reflect(-V, N);
    return texture(skybox, uvw).rgb;
}

vec3 blend3(vec3 x)
{
    vec3 y = 1.0 - x * x;
    y = max(y, vec3(0.0));
    return y;
}

void main()
{
    const vec3 center = vec3(0.0);
    if (length(geomOut.fragPosWS.xyz - center) < radius * 0.2)
    {
        discard;
    }

    const vec3 L = normalize(vec3(1.0, 1.0, 1.0));
    vec3 P = geomOut.fragPosWS.xyz;
    vec3 V = normalize(camPosWS - P);
    vec3 H = L + V;
    vec3 N = normalize(geomOut.normalWS);
    vec3 T = normalize(geomOut.tangentWS);

    float u = dot(T, H) * d;
    float w = dot(N, H);
    float e = r * u / w;
    float c = exp(-e * e);

    vec3 baseColor = SampleSkybox(V, N);
    vec3 anis = baseColor * c;

    if (u < 0.0)
    {
        u = -u;
    }
    vec3 cdiff = vec3(0.0);
    for (int n = 1; n < 8; ++n)
    {
        float y = 2 * u / n - 1;
        cdiff += blend3(vec3(4.0 * (y - 0.75), 4.0 * (y - 0.5), 4.0 * (y - 0.25)));
    }

    outColor = vec4(cdiff + anis, 1.0);
}
