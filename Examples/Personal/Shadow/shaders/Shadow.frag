#version 450 core

// reference: https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Smooth_Specular_Highlights

layout(location = 0) out vec4 outColor;
layout(location = 0) in VERTOUT
{
    vec3 normalWS;
    vec3 tangentWS;
    vec3 bitangentWS;
    vec2 texCoords;
    vec4 fragPosWS;
}
vertOut;

// color maps
uniform sampler2D map_AMBIENT;
uniform bool map_AMBIENT_exists;
uniform sampler2D map_DIFFUSE;
uniform bool map_DIFFUSE_exists;
uniform sampler2D map_SPECULAR;
uniform bool map_SPECULAR_exists;
uniform sampler2D map_EMISSIVE;
uniform bool map_EMISSIVE_exists;
// normal maps
uniform sampler2D map_NORMALS;
uniform bool map_NORMALS_exists;
// opacity maps
uniform sampler2D map_OPACITY;
uniform bool map_OPACITY_exists;
// color values
uniform vec3 val_AMBIENT;
uniform vec3 val_DIFFUSE;
uniform vec3 val_SPECULAR;
uniform vec3 val_EMISSIVE;
uniform float val_SHININESS;
uniform float val_OPACITY;
uniform float val_REFRACT;
uniform bool val_HASPBR;

// pbr maps
uniform sampler2D mapPBR_COLOR;
uniform bool mapPBR_COLOR_exists;
uniform sampler2D mapPBR_METALNESS;
uniform bool mapPBR_METALNESS_exists;
uniform sampler2D mapPBR_ROUGHNESS;
uniform bool mapPBR_ROUGHNESS_exists;
uniform sampler2D mapPBR_OCCLUSION;
uniform bool mapPBR_OCCLUSION_exists;
uniform float valPBR_METALLIC;
uniform float valPBR_ROUGHNESS;

struct DirLight
{
    float dir[3];
    float color[3];
    float intensity;
    int castShadow;
    float shadowStrength;
};

struct PointLight
{
    float pos[3];
    float range;
    float color[3];
    float intensity;
    int castShadow;
    float shadowStrength;
};

struct SpotLight
{
    float pos[3];
    float dir[3];
    float range;
    float cutoffInner;
    float cutoffOuter;
    float color[3];
    float intensity;
    int castShadow;
    float shadowStrength;
};

#define LIGHTS_MAX_DIR_LIGHTS 4
#define LIGHTS_MAX_POINT_LIGHTS 4
#define LIGHTS_MAX_SPOT_LIGHTS 4

// light data
layout(std430, binding = 1) readonly buffer LightsData
{
    uint dirLightsLen;
    DirLight dirLights[LIGHTS_MAX_DIR_LIGHTS];
    uint pointLightsLen;
    PointLight pointLights[LIGHTS_MAX_POINT_LIGHTS];
    uint spotLightsLen;
    SpotLight spotLights[LIGHTS_MAX_SPOT_LIGHTS];
};

// shadow data
#define SHADOW_SIZE 1024
#define SHADOW_SIZE_INV (1.0 / SHADOW_SIZE)
#define SHADOW_CSM_COUNT 4
uniform sampler2DArray map_DirShadow;
layout(std430, binding = 2) readonly buffer ShadowDirData
{
    vec4 shadowDirData[SHADOW_CSM_COUNT];
};
layout(std430, binding = 3) readonly buffer ShadowDirLightData
{
    mat4 shadowDirLightData[SHADOW_CSM_COUNT * LIGHTS_MAX_DIR_LIGHTS];
};
uniform samplerCubeArray map_PointShadow;
uniform vec2 shadowPointData;

// other uniforms
uniform vec3 vec_CameraPosWS;

#define PI 3.14159265359

struct Surface
{
    vec3 colorAmbient;
    vec3 colorDiffuse;
    vec3 colorSpecular;
    vec3 colorEmissive;
    vec3 normDir;
    vec3 viewDir;
    vec3 fragPos;
    vec2 texCoords;
    float opacity;
    float metallic;
    float roughness;
    float ao;
};

float Random(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 RandomCoord(vec2 co, float val)
{
    return vec2(Random(co), Random(co.yx * val));
}

vec3 RandomCoord3(vec3 co, float val)
{
    return vec3(Random(co.xy), Random(co.zx * val), Random(vec2(co.y, val)));
}

vec3 ComputeNormal()
{
    if (map_NORMALS_exists)
    {
        mat3 TBN = mat3(vertOut.tangentWS, vertOut.bitangentWS, vertOut.normalWS);
        // prepare normal tangent space
        vec3 normalTS = texture(map_NORMALS, vertOut.texCoords).xyz * 2.0 - 1.0;
        return normalize(TBN * normalTS);
    }
    else
    {
        return normalize(vertOut.normalWS);
    }
}

vec3 GetAmbientColor()
{
    return val_AMBIENT * (map_AMBIENT_exists ? texture(map_AMBIENT, vertOut.texCoords).rgb : vec3(0.25));
}

vec3 GetDiffuseColor()
{
    if (val_HASPBR && mapPBR_COLOR_exists)
    {
        return texture(mapPBR_COLOR, vertOut.texCoords).rgb;
    }
    else
    {
        return val_DIFFUSE * (map_DIFFUSE_exists ? texture(map_DIFFUSE, vertOut.texCoords).rgb : vec3(0.75));
    }
}

vec3 GetSpecularColor()
{
    return val_SPECULAR * (map_SPECULAR_exists ? texture(map_SPECULAR, vertOut.texCoords).rgb : vec3(1.0));
}

vec3 GetEmissiveColor()
{
    return val_EMISSIVE * (map_EMISSIVE_exists ? texture(map_EMISSIVE, vertOut.texCoords).rgb : vec3(1.0));
}

float GetOcclusion()
{
    return mapPBR_OCCLUSION_exists ? texture(mapPBR_OCCLUSION, vertOut.texCoords).r : 1.0;
}

float GetOpacity()
{
    return val_OPACITY * (map_OPACITY_exists ? texture(map_OPACITY, vertOut.texCoords).r : 1.0);
}

float GetMetallic()
{
    return valPBR_METALLIC * (mapPBR_METALNESS_exists ? texture(mapPBR_METALNESS, vertOut.texCoords).b : 1.0);
}

float GetRoughness()
{
    return valPBR_ROUGHNESS * (mapPBR_ROUGHNESS_exists ? texture(mapPBR_ROUGHNESS, vertOut.texCoords).g : 1.0);
}

void ComputeDirLight(DirLight light, Surface surface, out float diff, out float spec)
{
    vec3 lightDir = normalize(vec3(light.dir[0], light.dir[1], light.dir[2]));
    // diffuse
    diff = max(dot(surface.normDir, lightDir), 0.0);
    // specular
    spec = val_SHININESS > 0.0 ? pow(max(dot(reflect(-lightDir, surface.normDir), surface.viewDir), 0.0), val_SHININESS)
                               : 0.0;
}

float ComputeLightAttenuation(float range, vec3 fragToLight)
{
    float distSqr = dot(fragToLight, fragToLight);
    float invRSqr = (range == 0.0) ? 1.0 : (1.0 / (range * range));
    float inner = distSqr * invRSqr;
    inner = clamp(1.0 - inner * inner, 0.0, 1.0);
    return inner * inner;
}

void ComputePointLight(PointLight light, Surface surface, out float diff, out float spec, out float atten)
{
    vec3 fragToLight = vec3(light.pos[0], light.pos[1], light.pos[2]) - surface.fragPos;
    vec3 lightDir = normalize(fragToLight);
    // diffuse
    diff = max(dot(surface.normDir, lightDir), 0.0);
    // specular
    spec = val_SHININESS > 0.0 ? pow(max(dot(reflect(-lightDir, surface.normDir), surface.viewDir), 0.0), val_SHININESS)
                               : 0.0;
    // attenuation
    atten = ComputeLightAttenuation(light.range, fragToLight);
}

void ComputeSpotLight(SpotLight light, Surface surface, out float diff, out float spec, out float atten)
{
    vec3 fragToLight = vec3(light.pos[0], light.pos[1], light.pos[2]) - surface.fragPos;
    vec3 toLight = normalize(fragToLight);
    vec3 lightDir = normalize(vec3(light.dir[0], light.dir[1], light.dir[2]));
    // diffuse
    diff = max(dot(surface.normDir, toLight), 0.0);
    // specular
    spec = val_SHININESS > 0.0 ? pow(max(dot(reflect(-toLight, surface.normDir), surface.viewDir), 0.0), val_SHININESS)
                               : 0.0;
    // compute intensity
    float theta = dot(toLight, -lightDir);
    atten = clamp((theta - light.cutoffOuter) / (light.cutoffOuter - light.cutoffInner), 0.0, 1.0) *
            ComputeLightAttenuation(light.range, fragToLight);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) * 0.125;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    float val = clamp(1.0 - cosTheta, 0.0, 1.0);
    return F0 + (1.0 - F0) * val * val * val * val * val;
}

vec3 ComputeLightPBR(Surface surface, vec3 lightDir, vec3 lightColor)
{
    vec3 H = normalize(surface.viewDir + lightDir);
    // cook-torrance brdf
    float NDF = DistributionGGX(surface.normDir, H, surface.roughness);
    float G = GeometrySmith(surface.normDir, surface.viewDir, lightDir, surface.roughness);
    vec3 kS = clamp(fresnelSchlick(max(dot(H, surface.viewDir), 0.0), surface.colorSpecular), 0.0, 1.0);
    vec3 kD = vec3(1.0) - kS;
    float NdotV = max(dot(surface.normDir, surface.viewDir), 0.0);
    float NdotL = max(dot(surface.normDir, lightDir), 0.0);
    vec3 spec = NDF * G * kS / (4.0 * NdotV * NdotL + 1e-5);
    return (kD * surface.colorDiffuse + spec) * NdotL * lightColor;
}

vec3 ComputeLightPBR2(Surface surface, vec3 lightDir, vec3 lightColor)
{
    float lightF = clamp(dot(surface.normDir, lightDir), 0.0, 1.0);
    float brdfF;
    {
        vec3 H = normalize(surface.viewDir + lightDir);
        float NH2 = clamp(dot(surface.normDir, H), 0.0, 1.0);
        NH2 = NH2 * NH2;
        float LH2 = clamp(dot(lightDir, H), 0.0, 1.0);
        LH2 = LH2 * LH2;
        float r2 = surface.roughness * surface.roughness;
        float d2 = NH2 * (r2 * r2 - 1.0) + 1e-5 + 1.0;
        d2 = d2 * d2;
        float norm = r2 * 4.0 + 2.0;
        brdfF = r2 * r2 / (d2 * max(1e-5, LH2) * norm);
    }
    return (surface.colorDiffuse + brdfF * surface.colorSpecular) * lightColor * lightF;
}

float ComputeDirShadowAtten(Surface surface, int lightIdx, float shadowStrength)
{
    // find cascade index
    int cascadeIdx;
    float dist = 1.0, sphereW = 0.0;
    for (cascadeIdx = 0; cascadeIdx < SHADOW_CSM_COUNT; ++cascadeIdx)
    {
        vec4 sphere = shadowDirData[cascadeIdx];
        dist = dot(surface.fragPos - sphere.xyz, surface.fragPos - sphere.xyz);
        sphereW = sphere.w * sphere.w;
        if (dist < sphereW)
            break;
    }
    // validate cascade index
    if (cascadeIdx == SHADOW_CSM_COUNT || shadowStrength == 0.0)
        return 1.0;
    // sample shadow maps
    int layerIdx = lightIdx * SHADOW_CSM_COUNT + cascadeIdx;
    vec4 pos = shadowDirLightData[layerIdx] * vec4(surface.fragPos + surface.normDir * 0.001, 1.0);
    pos.xyz = pos.xyz * 0.5 + 0.5;
    float shadow = 0.0;
    {
        for (int x = -4; x < 4; x += 4)
        {
            for (int y = -4; y < 4; y += 4)
            {
                vec2 co = vec2(x, y);
                vec2 offset = RandomCoord(co * (shadow + pos.zx), (shadow + 1.0) * pos.y) * 3.0 - 1.0;
                shadow +=
                    float(pos.z <= texture(map_DirShadow, vec3(pos.xy + (co + offset) * SHADOW_SIZE_INV, layerIdx)).r) *
                    0.25;
            }
        }
    }
    // more sampling for soft edge shadow
    if (shadow * (1.0 - shadow) != 0.0)
    {
        shadow *= 4.0;
        for (int x = -4; x < 4; x += 2)
        {
            for (int y = -4; y < 4; y += 2)
            {
                vec2 co = vec2(x, y);
                vec2 offset = RandomCoord(co * (shadow + pos.zx), (shadow + 1.0) * pos.y) * 3.0 - 1.0;
                shadow +=
                    float(pos.z <= texture(map_DirShadow, vec3(pos.xy + (co + offset) * SHADOW_SIZE_INV, layerIdx)).r);
            }
        }
        shadow *= 0.05;
    }
    return mix(1.0, shadow, shadowStrength);
}

float GetTrueDepth(float sampled, float zNear, float zFar)
{
    float depth = 2.0 * sampled - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - depth * (zFar - zNear));
}

float ComputePointShadowAtten(Surface surface, vec3 lightPos, int lightIdx, float shadowStrength)
{
    if (shadowStrength == 0.0)
        return 1.0;
    vec3 lightToSurface = surface.fragPos + 0.005 * surface.normDir - lightPos;
    float trueDepth = length(lightToSurface);
    trueDepth = trueDepth * trueDepth;
    float shadow = 0.0, depth;
    // for (int x = -2; x < 2; x += 2)
    // {
    //     for (int y = -2; y < 2; y += 2)
    //     {
    //         for (int z = -2; z < 2; z += 2)
    //         {
    //             vec3 co = vec3(x, y, z);
    //             vec3 offset = RandomCoord3(co * (shadow + surface.fragPos.zxy), (shadow + 1.0) * surface.fragPos.y);
    //             depth = GetTrueDepth(
    //                 texture(map_PointShadow, vec4(lightToSurface + (co + offset) * SHADOW_SIZE_INV, lightIdx)).r,
    //                 shadowPointData.x, shadowPointData.y);
    //             shadow += float(trueDepth <= depth) * 0.125;
    //         }
    //     }
    // }
    depth =
        GetTrueDepth(texture(map_PointShadow, vec4(lightToSurface, lightIdx)).r, shadowPointData.x, shadowPointData.y);
    shadow += float(trueDepth <= depth);
    return mix(1.0, shadow, shadowStrength);
}

void main()
{
    Surface surface;
    surface.colorAmbient = GetAmbientColor();
    surface.colorEmissive = GetEmissiveColor();
    surface.normDir = ComputeNormal();
    surface.viewDir = normalize(vec_CameraPosWS - vertOut.fragPosWS.xyz);
    surface.fragPos = vertOut.fragPosWS.xyz;
    surface.texCoords = vertOut.texCoords;
    surface.opacity = GetOpacity();
    surface.metallic = GetMetallic();
    surface.roughness = GetRoughness();
    surface.ao = GetOcclusion();

    if (val_HASPBR)
    {
        vec3 color = GetDiffuseColor();
        surface.colorDiffuse = mix(color * 0.96, vec3(0.0), surface.metallic);
        surface.colorSpecular = mix(vec3(0.04), color, surface.metallic);
    }
    else
    {
        surface.colorDiffuse = GetDiffuseColor();
        surface.colorSpecular = GetSpecularColor();
    }

    if (dirLightsLen + pointLightsLen + spotLightsLen == 0)
    {
        outColor = vec4(surface.colorDiffuse, surface.opacity);
        return;
    }

    vec3 accColor = vec3(0.0);
    if (val_HASPBR)
    {
        // go PBR path
        for (uint i = 0; i < dirLightsLen; i++)
        {
            DirLight light = dirLights[i];
            vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]);
            vec3 lightDir = normalize(vec3(light.dir[0], light.dir[1], light.dir[2]));
            float shadow = light.castShadow > 0.0 ? ComputeDirShadowAtten(surface, int(i), light.shadowStrength) : 1.0;
            // accColor += ComputeLightPBR(surface, lightDir, lightColor * light.intensity) * shadow;
            accColor += ComputeLightPBR2(surface, lightDir, lightColor * light.intensity) * shadow;
        }

        for (uint i = 0; i < pointLightsLen; i++)
        {
            PointLight light = pointLights[i];
            vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]);
            vec3 lightPos = vec3(light.pos[0], light.pos[1], light.pos[2]);
            vec3 lightDir = lightPos - surface.fragPos;
            float shadow =
                light.castShadow > 0.0 ? ComputePointShadowAtten(surface, lightPos, int(i), light.shadowStrength) : 1.0;
            float atten = ComputeLightAttenuation(light.range, lightDir);
            // accColor += ComputeLightPBR(surface, normalize(lightDir), lightColor * light.intensity * atten);
            accColor += ComputeLightPBR2(surface, normalize(lightDir), lightColor * light.intensity * atten) * shadow;
        }

        for (uint i = 0; i < spotLightsLen; i++)
        {
            SpotLight light = spotLights[i];
            vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]);
            vec3 fragToLight = vec3(light.pos[0], light.pos[1], light.pos[2]) - surface.fragPos;
            vec3 lightDir = -normalize(vec3(light.dir[0], light.dir[1], light.dir[2]));
            float atten = ComputeLightAttenuation(light.range, fragToLight);
            atten *= clamp((dot(normalize(fragToLight), lightDir) - light.cutoffOuter) /
                               (light.cutoffOuter - light.cutoffInner),
                           0.0, 1.0);
            // accColor += ComputeLightPBR(surface, lightDir, lightColor * light.intensity * atten);
            accColor += ComputeLightPBR2(surface, lightDir, lightColor * light.intensity * atten);
        }
    }
    else
    {
        // go Phong path
        for (uint i = 0; i < dirLightsLen; i++)
        {
            DirLight light = dirLights[i];
            vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]);
            float diff, spec;
            ComputeDirLight(light, surface, diff, spec);
            float shadow = light.castShadow > 0.0 ? ComputeDirShadowAtten(surface, int(i), light.shadowStrength) : 1.0;
            accColor += surface.colorAmbient + (surface.colorDiffuse * diff + surface.colorSpecular * spec) *
                                                   light.intensity * lightColor * shadow;
        }

        for (uint i = 0; i < pointLightsLen; i++)
        {
            PointLight light = pointLights[i];
            vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]);
            vec3 lightPos = vec3(light.pos[0], light.pos[1], light.pos[2]);
            float shadow =
                light.castShadow > 0.0 ? ComputePointShadowAtten(surface, lightPos, int(i), light.shadowStrength) : 1.0;
            float diff, spec, atten;
            ComputePointLight(light, surface, diff, spec, atten);
            accColor += surface.colorAmbient + (surface.colorDiffuse * diff + surface.colorSpecular * spec) *
                                                   light.intensity * lightColor * atten * shadow;
        }

        for (uint i = 0; i < spotLightsLen; i++)
        {
            SpotLight light = spotLights[i];
            vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]);
            float diff, spec, atten;
            ComputeSpotLight(light, surface, diff, spec, atten);
            accColor += surface.colorAmbient + (surface.colorDiffuse * diff + surface.colorSpecular * spec) *
                                                   light.intensity * lightColor * atten;
        }
    }

    outColor = vec4(accColor * surface.ao + surface.colorEmissive, surface.opacity);
}
