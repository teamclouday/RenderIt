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
    vec3 viewTS;
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
// displacement maps
uniform sampler2D map_HEIGHT;
uniform bool map_HEIGHT_exists;
// color values
uniform vec3 val_AMBIENT;
uniform vec3 val_DIFFUSE;
uniform vec3 val_SPECULAR;
uniform vec3 val_EMISSIVE;
uniform float val_SHININESS;

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
layout(std430, binding = 1) buffer LightsData
{
    uint dirLightsLen;
    DirLight dirLights[LIGHTS_MAX_DIR_LIGHTS];
    uint pointLightsLen;
    PointLight pointLights[LIGHTS_MAX_POINT_LIGHTS];
    uint spotLightsLen;
    SpotLight spotLights[LIGHTS_MAX_SPOT_LIGHTS];
};

// other uniforms
uniform vec3 vec_CameraPosWS;
uniform float val_HeightScale;
uniform float val_HeightLayers;

vec3 ComputeNormal(vec2 texCoords)
{
    if (map_NORMALS_exists)
    {
        mat3 TBN = mat3(vertOut.tangentWS, vertOut.bitangentWS, vertOut.normalWS);
        // prepare normal tangent space
        vec3 normalTS = texture(map_NORMALS, texCoords).xyz * 2.0 - 1.0;
        return normalize(TBN * normalTS);
    }
    else
    {
        return normalize(vertOut.normalWS);
    }
}

vec3 GetAmbientColor(vec2 texCoords)
{
    return val_AMBIENT * (map_AMBIENT_exists ? texture(map_AMBIENT, texCoords).rgb : vec3(0.25));
}

vec3 GetDiffuseColor(vec2 texCoords)
{
    return val_DIFFUSE * (map_DIFFUSE_exists ? texture(map_DIFFUSE, texCoords).rgb : vec3(0.75));
}

vec3 GetSpecularColor(vec2 texCoords)
{
    return val_SPECULAR * (map_SPECULAR_exists ? texture(map_SPECULAR, texCoords).rgb : vec3(1.0));
}

vec3 GetEmissiveColor(vec2 texCoords)
{
    return val_EMISSIVE * (map_EMISSIVE_exists ? texture(map_EMISSIVE, texCoords).rgb : vec3(1.0));
}

void ComputeDirLight(DirLight light, vec3 normDir, vec3 viewDir, out float diff, out float spec)
{
    vec3 lightDir = normalize(vec3(light.dir[0], light.dir[1], light.dir[2]));
    // diffuse
    diff = max(dot(normDir, lightDir), 0.0);
    // specular
    spec = val_SHININESS > 0.0 ? pow(max(dot(reflect(-lightDir, normDir), viewDir), 0.0), val_SHININESS) : 0.0;
}

float ComputeLightAttenuation(float range, vec3 fragToLight)
{
    float distSqr = dot(fragToLight, fragToLight);
    float invRSqr = (range == 0.0) ? 1.0 : (1.0 / (range * range));
    float inner = distSqr * invRSqr;
    inner = clamp(1.0 - inner * inner, 0.0, 1.0);
    return inner * inner;
}

void ComputePointLight(PointLight light, vec3 normDir, vec3 viewDir, vec3 fragPos, out float diff, out float spec,
                       out float atten)
{
    vec3 fragToLight = vec3(light.pos[0], light.pos[1], light.pos[2]) - fragPos;
    vec3 lightDir = normalize(fragToLight);
    // diffuse
    diff = max(dot(normDir, lightDir), 0.0);
    // specular
    spec = val_SHININESS > 0.0 ? pow(max(dot(reflect(-lightDir, normDir), viewDir), 0.0), val_SHININESS) : 0.0;
    // attenuation
    atten = ComputeLightAttenuation(light.range, fragToLight);
}

void ComputeSpotLight(SpotLight light, vec3 normDir, vec3 viewDir, vec3 fragPos, out float diff, out float spec,
                      out float atten)
{
    vec3 fragToLight = vec3(light.pos[0], light.pos[1], light.pos[2]) - fragPos;
    vec3 toLight = normalize(fragToLight);
    vec3 lightDir = normalize(vec3(light.dir[0], light.dir[1], light.dir[2]));
    // diffuse
    diff = max(dot(normDir, toLight), 0.0);
    // specular
    spec = val_SHININESS > 0.0 ? pow(max(dot(reflect(-toLight, normDir), viewDir), 0.0), val_SHININESS) : 0.0;
    // compute intensity
    float theta = dot(toLight, -lightDir);
    atten = clamp((theta - light.cutoffOuter) / (light.cutoffOuter - light.cutoffInner), 0.0, 1.0) *
            ComputeLightAttenuation(light.range, fragToLight);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float layerDepth = 1.0 / val_HeightLayers;
    viewDir *= val_HeightScale * layerDepth / -viewDir.z;

    float currDepth = 0.0;
    vec2 prevTexCoords = texCoords, nextTexCoords = texCoords;
    float prevDepth = texture(map_HEIGHT, texCoords).r;
    float nextDepth = prevDepth;
    while (currDepth < nextDepth)
    {
        prevTexCoords = nextTexCoords;
        nextTexCoords = prevTexCoords + viewDir.xy;

        prevDepth = nextDepth;
        nextDepth = texture(map_HEIGHT, nextTexCoords).r;

        currDepth += layerDepth;
    }

    float r1 = currDepth - nextDepth;
    float r2 = prevDepth - (currDepth - layerDepth);
    float r = r1 / (r1 + r2);

    return mix(nextTexCoords, prevTexCoords, r);
}

void main()
{
    vec3 viewDir = normalize(vec_CameraPosWS - vertOut.fragPosWS.xyz);
    vec2 texCoords = map_HEIGHT_exists ? ParallaxMapping(vertOut.texCoords, vertOut.viewTS) : vertOut.texCoords;
    vec3 normDir = ComputeNormal(texCoords);

    vec3 colAmbient = GetAmbientColor(texCoords);
    vec3 colDiffuse = GetDiffuseColor(texCoords);
    vec3 colSpecular = GetSpecularColor(texCoords);

    vec3 accColor = vec3(0.0);
    // apply directional lights
    for (uint i = 0; i < dirLightsLen; i++)
    {
        DirLight light = dirLights[i];
        vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]);
        float diff, spec;
        ComputeDirLight(light, normDir, viewDir, diff, spec);
        accColor += colAmbient + (colDiffuse * diff + colSpecular * spec) * light.intensity * lightColor;
    }
    // apply point lights
    for (uint i = 0; i < pointLightsLen; i++)
    {
        PointLight light = pointLights[i];
        vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]);
        float diff, spec, atten;
        ComputePointLight(light, normDir, viewDir, vertOut.fragPosWS.xyz, diff, spec, atten);
        accColor += colAmbient + (colDiffuse * diff + colSpecular * spec) * light.intensity * lightColor * atten;
    }
    // apply spot lights
    for (uint i = 0; i < spotLightsLen; i++)
    {
        SpotLight light = spotLights[i];
        vec3 lightColor = vec3(light.color[0], light.color[1], light.color[2]);
        float diff, spec, atten;
        ComputeSpotLight(light, normDir, viewDir, vertOut.fragPosWS.xyz, diff, spec, atten);
        accColor += colAmbient + (colDiffuse * diff + colSpecular * spec) * light.intensity * lightColor * atten;
    }

    vec3 colEmissive = GetEmissiveColor(texCoords);

    if (dirLightsLen + pointLightsLen + spotLightsLen == 0)
    {
        accColor = colDiffuse;
    }

    outColor = vec4(accColor + colEmissive, 1.0);
}
