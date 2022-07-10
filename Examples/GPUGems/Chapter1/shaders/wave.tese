#version 450 core

layout(quads, equal_spacing, ccw) in;

layout(location = 0) out VERTOUT
{
    vec3 normWS;
    vec4 posWS;
}
vertOut;

#define SineWaveType_Directional 0
#define SineWaveType_Circular 1

struct SineWave
{
    int type;
    float len;
    float amp;
    float speed;
    float steepness;
    float dir[2];
    float center[2];
};

layout(std430, binding = 0) readonly buffer SineWaves
{
    uint wavesLen;
    SineWave waves[];
};

uniform mat4 mat_VP;
uniform mat4 mat_Model;
uniform mat3 mat_ModelInv;
uniform float val_Time;
uniform float val_K;

uniform bool enableGerstnerWave;

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

// W = A * sin(dot(D, (x,y)) * w + t * phi)
// dx(W) = w * D.x * A * cos(dot(D, (x,y)) * w + t * phi)
float ComputeHeightNormal(SineWave wave, vec2 pos, inout vec2 norm)
{
    // prepare
    float w = 2.0 / wave.len;
    float phi = wave.speed * w;
    vec2 D = GetDirection(wave, pos);
    // compute angle
    float angle = dot(D, pos) * w + val_Time * phi;
    // prepare normal
    float normVal = -w * wave.amp * cos(angle);
    norm = normVal * D;
    return wave.amp * sin(angle);
}

// generate sharper rolls
// W = 2A * ((sin(dot(D, (x,y)) * w + t * phi) + 1) / 2) ^ k
// dx(W) = k * w * D.x * A * cos(dot(D, (x,y)) * w + t * phi) *
//         ((sin(dot(D, (x,y)) * w + t * phi) + 1) / 2) ^ (k - 1)
float ComputeHeightNormalVariant(SineWave wave, vec2 pos, inout vec2 norm)
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
    norm = normVal * D;
    return 2.0 * wave.amp * power * toPower;
}

float ComputeHeightNormalAll(vec2 pos, inout vec3 norm)
{
    float ret = 0.0;
    vec2 n = vec2(0.0);
    norm = vec3(0.0, 0.0, 1.0);
    for (uint i = 0; i < wavesLen; i++)
    {
        ret += ComputeHeightNormalVariant(waves[i], pos, n);
        norm.xy += n;
    }
    norm = SafeNormalize(norm);
    return ret;
}

vec3 ComputeGerstnerWave(SineWave wave, vec2 pos, inout vec3 norm)
{
    // prepare
    float w = 2.0 / wave.len;
    float phi = wave.speed * w;
    vec2 D = GetDirection(wave, pos);
    float Q = wave.steepness;
    // prepare angle
    float angle = dot(D, pos) * w + val_Time * phi;
    float cosA = cos(angle);
    float sinA = sin(angle);
    // prepare normal
    norm.xy = -w * wave.amp * cosA * D;
    norm.z = -Q * w * wave.amp * sinA;
    return vec3(D * Q * wave.amp * cos(angle), wave.amp * sin(angle));
}

vec3 ComputeGerstnerWaveAll(vec2 pos, inout vec3 norm)
{
    vec3 ret = vec3(0.0);
    vec3 n = vec3(0.0);
    norm = vec3(0.0);
    for (uint i = 0; i < wavesLen; i++)
    {
        ret += ComputeGerstnerWave(waves[i], pos, n);
        norm += n;
    }
    norm.z = 1.0 - norm.z;
    norm = SafeNormalize(norm);
    return ret;
}

// stage 3
void main()
{
    vec2 uv = gl_TessCoord.xy;
    vec4 p0 = mix(gl_in[0].gl_Position, gl_in[3].gl_Position, uv.x);
    vec4 p1 = mix(gl_in[1].gl_Position, gl_in[2].gl_Position, uv.x);
    vec4 pos = mix(p0, p1, uv.y);

    vec3 norm = vec3(0.0, 0.0, 1.0);

    if (enableGerstnerWave)
    {
        vec3 newPos = ComputeGerstnerWaveAll(pos.xy, norm);
        pos.xy += newPos.xy;
        pos.z = newPos.z;
    }
    else
    {
        pos.z = ComputeHeightNormalAll(pos.xy, norm);
    }
    vertOut.normWS = normalize(norm * mat_ModelInv);
    vertOut.posWS = mat_Model * pos;
    gl_Position = mat_VP * vertOut.posWS;
}
