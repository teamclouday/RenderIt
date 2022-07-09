#version 450 core

layout(location = 0) out vec4 outColor;

layout(location = 0) in VERTOUT
{
    vec3 normalWS;
    vec4 fragPosWS;
}
vertOut;

// color values
uniform vec3 val_AMBIENT;
uniform vec3 val_DIFFUSE;
uniform vec3 val_SPECULAR;
uniform float val_SHININESS;

uniform vec3 vec_CameraPosWS;

void main()
{
    const vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 normDir = normalize(vertOut.normalWS);
    vec3 viewDir = normalize(vec_CameraPosWS - vertOut.fragPosWS.xyz);
    // diffuse
    float diff = max(dot(normDir, lightDir), 0.0);
    // specular
    float spec = val_SHININESS > 0.0 ? pow(max(dot(reflect(-lightDir, normDir), viewDir), 0.0), val_SHININESS) : 0.0;
    outColor = vec4(val_AMBIENT * val_DIFFUSE + val_DIFFUSE * diff + val_SPECULAR * spec, 1.0);
}
