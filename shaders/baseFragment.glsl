#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform float metallic;
uniform float roughness;
float ao = 0.5;

// lights
uniform vec3 lightPositions;
uniform vec3 lightColors;
uniform sampler2D texture1;
uniform vec3 camPos;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
// ----------------------------------------------------------------------------
void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);
    vec3 albedo = texture(texture1, TexCoords).xyz;
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    // calculate per-light radiance
    vec3 L = normalize(lightPositions - WorldPos);
    vec3 H = normalize(V + L);
    float distance = length(lightPositions - WorldPos);
    float attenuation = 1.0 ;// (distance * distance);
    vec3 radiance = lightColors * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}



//#version 330 core
//out vec4 FragColor;
//in vec2 texturePos;
//in vec3 pos;
//in vec3 normal;
//
//// material parameters
//uniform float metallic;
//uniform float roughness;
//// lights
//uniform vec3 lightPos;
//uniform vec3 lightColor;
//
//uniform vec3 cameraPos;
//uniform sampler2D texture1;
//
//const float PI = 3.14159265359;
//
//float specularD(vec3 normal, vec3 wi, vec3 wo, float roughness)
//{
//    float roughnessSquared = roughness * roughness;
//    roughnessSquared = roughnessSquared * roughnessSquared;
//    vec3 halfVector = normalize(wi + wo);
//    float d = PI * pow(pow(dot(normal, halfVector), 2) * (roughnessSquared - 1) + 1, 2);
//    return roughnessSquared / max(d, 0.0000001);
//}
//
//float specularG(vec3 n, vec3 wo, vec3 wi, float roughness)
//{
//    float r = (roughness + 1.0);
//    float k = (r*r) / 8.0;
//    float ggx2 = dot(normal, wo) / (dot(normal, wo) * (1.0 - k) + k);;
//    float ggx1 = dot(normal, wi) / (dot(normal, wi) * (1.0 - k) + k);
//
//    return ggx1 * ggx2;
//}
//
//vec3 specularF(vec3 wi, vec3 wo, vec3 refraction)
//{
//    vec3 halfVector = normalize(wi + wo);
//    return refraction + (1.0 - refraction) * pow(max(1.0 - dot(halfVector, wo), 0.0), 5.0);
//}
//
//
//vec3 getBRDFColorResponse(vec3 n, vec3 wo, vec3 wi, float roughness, vec3 refraction, vec3 radiance){
//    float NDF = specularD(n, wo, wi, roughness);
//    float G   = specularG(n, wo, wi, roughness);
//    vec3 F    = specularF(wo, wi, refraction);
//
//    vec3 baseResponse = NDF * G * F / (4 * max(dot(n, wo), 0.0) * max(dot(n, wi), 0.0));
//
//    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
//    float NdotL = max(dot(n, wi), 0.0);
//    vec3 Lo = (kD * texture(texture1, texturePos).xyz / PI + baseResponse) * radiance * NdotL;
//
//    vec3 ambient = vec3(0.03) * texture(texture1, texturePos).xyz;
//    return ambient + Lo;
//}
//
//void main()
//{
//    vec3 N = normalize(normal);
//    vec3 cameraDir = normalize(cameraPos - pos);
//
//    vec3 F0 = vec3(0.04);
//    F0 = mix(F0, texture(texture1, texturePos).xyz, metallic);
//    vec3 lightDir = normalize(lightPos - pos);
//    float distance = length(lightPos - pos);
//    float attenuation = 1.0;
//    vec3 radiance = lightColor * attenuation;
//
//    // Cook-Torrance BRDF
//
//    vec3 color = getBRDFColorResponse(N, cameraDir, lightDir, roughness, F0, radiance);
//
//    // HDR tonemapping
//    color = color / (color + vec3(1.0));
//    ////    // gamma correct
//    color = pow(color, vec3(1.0/2.4));
//
//    FragColor = vec4(color, 1.0);
//}
//
