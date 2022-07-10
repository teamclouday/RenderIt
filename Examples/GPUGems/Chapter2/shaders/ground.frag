#version 450 core

layout(location = 0) out vec4 outColor;
layout(location = 0) in VERTOUT
{
    vec3 normalWS;
    vec4 fragPosWS;
    vec2 texCoords;
}
vertOut;

uniform vec3 val_DIFFUSE;

uniform sampler2D lightMap;
uniform vec3 surfColor;

#define SineWaveType_Directional 0
#define SineWaveType_Circular 1

struct SineWave
{
    int type;
    float len;
    float amp;
    float speed;
    float dir[2];
    float center[2];
};

layout(std430, binding = 0) readonly buffer SineWaves
{
    uint wavesLen;
    SineWave waves[];
};

uniform float val_Time;
uniform float val_K;

vec2 SafeNormalize(vec2 v)
{
    float len = length(v);
    return len == 0.0 ? v : v / len;
}

vec3 SafeNormalize(vec3 v)
{
    float len = length(v);
    return len == 0.0 ? v : v / len;
}

vec2 GetDirection(SineWave wave, vec2 pos)
{
    if (wave.type == SineWaveType_Directional)
    {
        return SafeNormalize(vec2(wave.dir[0], wave.dir[1]));
    }
    else if (wave.type == SineWaveType_Circular)
    {
        return SafeNormalize(pos - vec2(wave.center[0], wave.center[1]));
    }
    else
    {
        return vec2(0.0);
    }
}

vec2 ComputeNormalVariant(SineWave wave, vec2 pos)
{
    // prepare
    float w = 2.0 / wave.len;
    float phi = wave.speed * w;
    vec2 D = GetDirection(wave, pos);
    // compute angle
    float angle = dot(D, pos) * w + val_Time * phi;
    // prepare power
    float toPower = (sin(angle) + 1.0) * 0.5;
    float power = val_K <= 1.0 ? 1.0 : pow(toPower, val_K - 1.0);
    // prepare normal
    float normVal = -val_K * w * wave.amp * power * cos(angle);
    return normVal * D;
}

vec3 ComputeNormalAll(vec2 pos)
{
    vec3 norm = vec3(0.0, 1.0, 0.0);
    for (uint i = 0; i < wavesLen; ++i)
    {
        norm.xz += ComputeNormalVariant(waves[i], pos);
    }
    norm.z = -norm.z;
    return SafeNormalize(norm);
}

// this assumes the ground normal is +Y
vec3 GetInterception(vec3 pos, vec3 norm, float surfToGround)
{
    float dist = (surfToGround - pos.y) / norm.y;
    return pos + norm * dist;
}

void main()
{
    vec2 pos = vertOut.texCoords * 2.0 - 1.0;
    pos.y = -pos.y;
    // get normal
    vec3 norm = ComputeNormalAll(pos);
    // get intercetion point
    vec3 intercept = GetInterception(vertOut.fragPosWS.xyz, norm, -0.8);

    outColor = vec4(val_DIFFUSE, 1.0);
    outColor.rgb += texture(lightMap, intercept.xy * 2.0).rgb * surfColor * 0.5;
}
