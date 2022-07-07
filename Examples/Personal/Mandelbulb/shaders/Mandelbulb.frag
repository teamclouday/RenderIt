#version 450 core

// reference: https://github.com/pedrotrschneider/shader-fractals/blob/main/3D/Mandelbulb.glsl
// reference: https://www.shadertoy.com/view/wdjGWR

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 vertCoords;

// camera info
uniform vec3 val_CameraPos;
uniform vec3 val_CameraForward;
uniform vec3 val_CameraUp;
uniform vec3 val_CameraRight;
uniform float val_CameraFovRadians;
uniform float val_CameraRatio;

// ray marching info
uniform float val_RayMarchMaxSteps;
uniform float val_RayMarchMaxDist;
// mandelbulb info
uniform int val_MSteps;
uniform float val_MMaxDist;

uniform float val_Time;

struct Ray
{
    vec3 pos;
    vec3 dir;
    float dist;
};

Ray CreateRay(vec2 d)
{
    d.x *= val_CameraRatio;
    d *= val_CameraFovRadians;
    vec3 dir = normalize(d.x * val_CameraRight + d.y * val_CameraUp + val_CameraForward);
    return Ray(val_CameraPos, dir, 0.0);
}

float Mandelbulb(vec3 pos)
{
    vec3 z = pos;
    float r = 0.0;
    float dr = 1.0;
    float power = 3.0 + 5.0 * (sin(val_Time * 10.0) + 1.0);
    float theta, phi, zr;
    for (int i = 0; i < val_MSteps; ++i)
    {
        r = length(z);
        if (r > val_MMaxDist)
        {
            break;
        }
        // to polar coordinates
        theta = acos(z.z / r);
        phi = atan(z.y, z.x);
        dr = pow(r, power - 1.0) * power * dr + 1.0;
        // scale & rotate
        float zr = pow(r, power);
        theta *= power;
        phi *= power;
        // to cartesian coordinates
        z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
        z += pos;
    }
    return 0.5 * log(r) * r / dr;
}

vec3 hsv2rgb(vec3 c)
{
    const vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    // create ray
    Ray ray = CreateRay(vertCoords * 2.0 - 1.0);
    // prepare ray marching
    vec3 currPos, minDistPos = ray.pos;
    bool rayhit = false;
    // do ray marching
    float marchStep, dist, minDist = 1000.0;
    for (marchStep = 0.0; marchStep < val_RayMarchMaxSteps; ++marchStep)
    {
        currPos = ray.pos + ray.dir * ray.dist;
        dist = Mandelbulb(currPos);
        ray.dist += dist;
        if (minDist > dist)
        {
            minDist = dist;
            minDistPos = currPos;
        }
        if (dist < 1e-4)
        {
            rayhit = true;
            break;
        }
        else if (dist > val_RayMarchMaxDist)
        {
            break;
        }
    }
    // compute color
    vec3 color = vec3(0.0);
    if (rayhit)
    {
        color += vec3(0.8 + (length(currPos) * 2.0), 1.0, 0.8);
        color = hsv2rgb(color);
    }
    else
    {
        color += vec3(0.8 + (length(minDistPos) * 2.0), 1.0, 0.8);
        color = hsv2rgb(color);
        color *= 1.0 / (marchStep * minDist);
    }
    outColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}
