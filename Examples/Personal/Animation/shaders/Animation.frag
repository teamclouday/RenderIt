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

uniform sampler2D map_AMBIENT;
uniform bool map_AMBIENT_exists;
uniform sampler2D map_DIFFUSE;
uniform bool map_DIFFUSE_exists;
uniform sampler2D map_SPECULAR;
uniform bool map_SPECULAR_exists;
uniform sampler2D map_EMISSIVE;
uniform bool map_EMISSIVE_exists;

uniform sampler2D map_NORMALS;
uniform bool map_NORMALS_exists;

uniform vec3 val_AMBIENT;
uniform vec3 val_DIFFUSE;
uniform vec3 val_SPECULAR;
uniform vec3 val_EMISSIVE;

uniform float val_SHININESS;

uniform vec3 vec_cameraPos;

void main()
{
    vec3 normDir;
    if (map_NORMALS_exists)
    {
        mat3 TBN = mat3(vertOut.tangentWS, vertOut.bitangentWS, vertOut.normalWS);
        // prepare normal tangent space
        vec3 normalTS = texture(map_NORMALS, vertOut.texCoords).xyz * 2.0 - 1.0;
        normDir = normalize(TBN * normalTS);
    }
    else
    {
        normDir = normalize(vertOut.normalWS);
    }

    vec3 viewDir = normalize(vec_cameraPos - vertOut.fragPosWS.xyz);

    const vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    const vec3 lightColor = vec3(1.0, 1.0, 1.0);

    vec3 colAmbient = val_AMBIENT * (map_AMBIENT_exists ? texture(map_AMBIENT, vertOut.texCoords).rgb : vec3(0.1));

    vec3 colDiffuse = val_DIFFUSE * (map_DIFFUSE_exists ? texture(map_DIFFUSE, vertOut.texCoords).rgb : vec3(0.75));
    float intensity = dot(normDir, lightDir);
    colDiffuse *= lightColor * max(intensity, 0.0);

    vec3 colSpecular = vec3(0.0);
    if (intensity > 0.0)
    {
        colSpecular = (map_SPECULAR_exists ? texture(map_SPECULAR, vertOut.texCoords).rgb : vec3(1.0));
        colSpecular *= lightColor * pow(max(0.0, dot(reflect(-lightDir, normDir), viewDir)),
                                        val_SHININESS <= 0.0 ? 32.0 : val_SHININESS);
    }

    vec3 colEmissive = val_EMISSIVE * (map_EMISSIVE_exists ? texture(map_EMISSIVE, vertOut.texCoords).rgb : vec3(0.0));

    outColor = vec4(colAmbient + colDiffuse + colSpecular + colEmissive, 1.0);
}
