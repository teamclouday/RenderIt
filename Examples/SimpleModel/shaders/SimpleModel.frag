#version 450 core

// reference: https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Smooth_Specular_Highlights

layout (location = 0) out vec4 outColor;
layout (location = 0) in VERTOUT
{
    vec3 normalWS;
    vec2 texCoords;
    vec4 color;
    vec4 fragPosWS;
} vertOut;

uniform sampler2D map_AMBIENT;
uniform bool map_AMBIENT_exists;
uniform sampler2D map_DIFFUSE;
uniform bool map_DIFFUSE_exists;
uniform sampler2D map_SPECULAR;
uniform bool map_SPECULAR_exists;

uniform vec3 cameraPosWS;

void main()
{
    vec3 normDir = normalize(vertOut.normalWS);
    vec3 viewDir = normalize(cameraPosWS - vertOut.fragPosWS.xyx);

    const vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    const vec3 lightColor = vec3(1.0, 1.0, 1.0);

    vec3 colAmbient = map_AMBIENT_exists ?
        texture(map_AMBIENT, vertOut.texCoords).rgb :
        vertOut.color.rgb * 0.1;

    vec3 colDiffuse = map_DIFFUSE_exists ?
        texture(map_DIFFUSE, vertOut.texCoords).rgb :
        vertOut.color.rgb * 0.8;
    float intensity = dot(normDir, lightDir);
    colDiffuse *= lightColor * max(intensity, 0.0);

    vec3 colSpecular = vec3(0.0);
    if(intensity > 0.0)
    {
        colSpecular = map_SPECULAR_exists ?
            texture(map_SPECULAR, vertOut.texCoords).rgb :
            vertOut.color.rgb * 1.0;
        colSpecular *= lightColor * pow(max(0.0, dot(reflect(-lightDir, normDir), viewDir)), 32);
    }

    outColor = vec4(colAmbient + colDiffuse + colSpecular, 1.0);
}
