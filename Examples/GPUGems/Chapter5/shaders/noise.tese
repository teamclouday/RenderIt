#version 450 core

layout(triangles, equal_spacing, ccw) in;

layout(location = 0) in TESSCDATA
{
    vec3 normal;
    vec2 texCoords;
}
tesscOut[];

layout(location = 0) out TESSEDATA
{
    vec3 normal;
    float height;
    vec4 fragPos;
}
tesseOut;

uniform float controlTime;
uniform float controlRes;
uniform mat3 mat_ModelInv;
uniform mat4 mat_Model;
uniform mat4 mat_ProjView;
uniform int controlType;

#define TYPE_LUMPY 0
#define TYPE_MARBLED 1
#define TYPE_CRINKLED 2

#define PI 3.1415926538

// reference: http://www.science-and-fiction.org/rendering/noise.html

float rand2D(in vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
float rand3D(in vec3 co)
{
    return fract(sin(dot(co.xyz, vec3(12.9898, 78.233, 144.7272))) * 43758.5453);
}

float simple_interpolate(in float a, in float b, in float x)
{
    return a + smoothstep(0.0, 1.0, x) * (b - a);
}

float interpolatedNoise3D(in float x, in float y, in float z)
{
    float integer_x = x - fract(x);
    float fractional_x = x - integer_x;

    float integer_y = y - fract(y);
    float fractional_y = y - integer_y;

    float integer_z = z - fract(z);
    float fractional_z = z - integer_z;

    float v1 = rand3D(controlTime + vec3(integer_x, integer_y, integer_z));
    float v2 = rand3D(controlTime + vec3(integer_x + 1.0, integer_y, integer_z));
    float v3 = rand3D(controlTime + vec3(integer_x, integer_y + 1.0, integer_z));
    float v4 = rand3D(controlTime + vec3(integer_x + 1.0, integer_y + 1.0, integer_z));

    float v5 = rand3D(controlTime + vec3(integer_x, integer_y, integer_z + 1.0));
    float v6 = rand3D(controlTime + vec3(integer_x + 1.0, integer_y, integer_z + 1.0));
    float v7 = rand3D(controlTime + vec3(integer_x, integer_y + 1.0, integer_z + 1.0));
    float v8 = rand3D(controlTime + vec3(integer_x + 1.0, integer_y + 1.0, integer_z + 1.0));

    float i1 = simple_interpolate(v1, v5, fractional_z);
    float i2 = simple_interpolate(v2, v6, fractional_z);
    float i3 = simple_interpolate(v3, v7, fractional_z);
    float i4 = simple_interpolate(v4, v8, fractional_z);

    float ii1 = simple_interpolate(i1, i2, fractional_x);
    float ii2 = simple_interpolate(i3, i4, fractional_x);

    return simple_interpolate(ii1, ii2, fractional_y);
}

float Noise3D(in vec3 coord, in float wavelength)
{
    return interpolatedNoise3D(coord.x / wavelength, coord.y / wavelength, coord.z / wavelength);
}

// STRIPES TEXTURE (GOOD FOR MAKING MARBLE)
float stripes(float x, float f)
{
    float t = 0.5 + 0.5 * sin(f * 2.0 * PI * x);
    return t * t - 0.5;
}

// TURBULENCE TEXTURE
float turbulence(vec3 v, float f)
{
    float t = 0.0;
    for (; f <= controlRes; f *= 2.0)
    {
        t += abs(Noise3D(v, controlRes / f) / f);
    }
    return t;
}

float F_LUMPY(vec3 v)
{
    return 0.8 * Noise3D(v, 8.0);
}

float F_MARBLED(vec3 v)
{
    return 0.8 * stripes(v.x / controlRes + 2.0 * turbulence(v, 0.8), 1.6);
}

float F_CRINKLED(vec3 v)
{
    return 4.0 * turbulence(v, 4.0);
}

void main()
{
    // position
    vec4 pos = gl_TessCoord.x * gl_in[0].gl_Position + // 0
               gl_TessCoord.y * gl_in[1].gl_Position + // 1
               gl_TessCoord.z * gl_in[2].gl_Position;  // 2
    // normal
    vec3 norm = normalize(gl_TessCoord.x * tesscOut[0].normal + // 0
                          gl_TessCoord.y * tesscOut[1].normal + // 1
                          gl_TessCoord.z * tesscOut[2].normal); // 2
    // tex coords
    vec2 texCoords = gl_TessCoord.x * tesscOut[0].texCoords + // 0
                     gl_TessCoord.y * tesscOut[1].texCoords + // 1
                     gl_TessCoord.z * tesscOut[2].texCoords;  // 2
    // compute normal
    const float e = 0.01, eInv = 100;
    float F0 = 0.0, Fx = 0.0, Fy = 0.0, Fz = 0.0;
    vec3 p = controlRes * (pos.xyz + 1.0);
    if (controlType == TYPE_LUMPY)
    {
        F0 = F_LUMPY(p);
        Fx = F_LUMPY(p + vec3(e, 0.0, 0.0));
        Fy = F_LUMPY(p + vec3(0.0, e, 0.0));
        Fz = F_LUMPY(p + vec3(0.0, 0.0, e));
    }
    else if (controlType == TYPE_MARBLED)
    {
        F0 = F_MARBLED(p);
        Fx = F_MARBLED(p + vec3(e, 0.0, 0.0));
        Fy = F_MARBLED(p + vec3(0.0, e, 0.0));
        Fz = F_MARBLED(p + vec3(0.0, 0.0, e));
    }
    else
    {
        F0 = F_CRINKLED(p);
        Fx = F_CRINKLED(p + vec3(e, 0.0, 0.0));
        Fy = F_CRINKLED(p + vec3(0.0, e, 0.0));
        Fz = F_CRINKLED(p + vec3(0.0, 0.0, e));
    }
    vec3 dF = vec3((Fx - F0) * eInv, (Fy - F0) * eInv, (Fz - F0) * eInv);
    tesseOut.normal = normalize(norm - dF) * mat_ModelInv;
    // modify height
    tesseOut.height = F0;
    // modify position
    pos.xyz += norm * tesseOut.height * 0.1;
    tesseOut.fragPos = mat_Model * pos;
    gl_Position = mat_ProjView * tesseOut.fragPos;
}
