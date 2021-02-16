#version 330 core
out vec4 FragColor;
in vec2 texturePos;
in vec3 pos;
in vec3 normal;

// material parameters
uniform float metallic;
uniform float roughness;
// lights
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 cameraPos;
uniform sampler2D texture1;

const float PI = 3.14159265359;

float specularD(vec3 normal, vec3 wi, vec3 wo, float roughness)
{
    float roughnessSquared = roughness * roughness;
    roughnessSquared = roughnessSquared * roughnessSquared;
    vec3 halfVector = normalize(wi + wo);
    float d = PI * pow(pow(dot(normal, halfVector), 2) * (roughnessSquared - 1) + 1, 2);
    return roughnessSquared / max(d, 0.0000001);
}

float specularG(vec3 n, vec3 wo, vec3 wi, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float ggx2 = dot(normal, wo) / (dot(normal, wo) * (1.0 - k) + k);;
    float ggx1 = dot(normal, wi) / (dot(normal, wi) * (1.0 - k) + k);

    return ggx1 * ggx2;
}

vec3 specularF(vec3 wi, vec3 wo, vec3 refraction)
{
    vec3 halfVector = normalize(wi + wo);
    return refraction + (1.0 - refraction) * pow(max(1.0 - dot(halfVector, wo), 0.0), 5.0);
}


vec3 getBRDFColorResponse(vec3 n, vec3 wo, vec3 wi, float roughness, vec3 refraction, vec3 radiance){
    float NDF = specularD(n, wo, wi, roughness);
    float G   = specularG(n, wo, wi, roughness);
    vec3 F    = specularF(wo, wi, refraction);

    vec3 baseResponse = NDF * G * F / (4 * max(dot(n, wo), 0.0) * max(dot(n, wi), 0.0));

    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
    float NdotL = max(dot(n, wi), 0.0);
    vec3 Lo = (kD * texture(texture1, texturePos).xyz / PI + baseResponse) * radiance * NdotL;

    vec3 ambient = vec3(0.03) * texture(texture1, texturePos).xyz;
    return ambient + Lo;
}

void main()
{
    vec3 N = normalize(normal);
    vec3 cameraDir = normalize(cameraPos - pos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, texture(texture1, texturePos).xyz, metallic);
    vec3 lightDir = normalize(lightPos - pos);
    float distance = length(lightPos - pos);
    float attenuation = 1.0;
    vec3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF

    vec3 color = getBRDFColorResponse(N, cameraDir, lightDir, roughness, F0, radiance);

    // HDR tonemapping
    //    color = color / (color + vec3(1.0));
    ////    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}

