#version 330 core
out vec4 FragColor;
in vec2 texCoord;
in vec3 worldPos;
in vec3 worldNormal;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 camPos;

//object textures
uniform sampler2D textureBaseColor;
uniform sampler2D textureNormal; //todo
uniform sampler2D textureMetallic;
uniform sampler2D textureRoughness;

const float PI = 3.14159265359;

float specularD(vec3 worldNormal, vec3 wi, vec3 wo, float roughness) {
    float a = roughness * roughness;
    a = a * a;
    vec3 halfVector = normalize(wi + wo);
    float d = PI * pow(pow(dot(worldNormal, halfVector), 2) * (a - 1) + 1, 2);
    return a / max(d, 0.0000001);
}

float specularG(vec3 n, vec3 wi, vec3 wo, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float ggx2 = dot(n, wo) / (dot(n, wo) * (1.0 - k) + k);
    float ggx1 = dot(n, wi) / (dot(n, wi) * (1.0 - k) + k);

    return ggx1 * ggx2;
}

vec3 specularF(vec3 wi, vec3 wo, vec3 refraction) {
    vec3 halfVector = normalize(wi + wo);
    return refraction + (1.0 - refraction) * pow(max(1.0 - dot(halfVector, wo), 0.0), 5.0);
}

vec3 calculateReflectance(vec3 albedo, float metallic) {
    vec3 reflectance = vec3(0.04);
    reflectance = mix(reflectance, albedo, metallic);
    return reflectance;
}


void main() {
    // Data preparation
    vec3 normal = normalize(worldNormal);
    vec3 wi = normalize(camPos - worldPos);// in direction
    vec3 wo = normalize(lightPosition - worldPos);// out direction
    vec3 albedo = texture(textureBaseColor, texCoord).xyz;
    float metallic = texture(textureMetallic, texCoord).x;
    float roughness = texture(textureRoughness, texCoord).x;
    vec3 reflectance = calculateReflectance(albedo, metallic);

    // BRDF
    float NDF = specularD(normal, wi, wo, roughness);
    float G = specularG(normal, wi, wo, roughness);
    vec3 F = specularF(wi, wo, reflectance);

    vec3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(normal, wi), 0.0) * max(dot(normal, wo), 0.0);
    vec3 specular = nominator / max(denominator, 0.000001);

    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);

    vec3 radiance = (kD * albedo / PI + specular) * lightColor * dot(normal, wo);
    vec3 ambient = vec3(0.03) * albedo;

    vec3 color = ambient + radiance;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.0));

    FragColor = vec4(color, 1.0);
    FragColor = texture(textureBaseColor, texCoord);
}
