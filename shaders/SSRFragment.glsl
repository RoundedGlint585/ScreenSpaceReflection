#version 330 core

uniform sampler2D tFrame;
uniform sampler2D tDepth;
uniform sampler2D tNorm;
uniform sampler2D tMetallic;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 invProj;

uniform float rayStep = 0.2f;
uniform int iterationCount = 100;

vec3 generatePositionFromDepth(vec2 texturePos, float depth){
    vec4 ndc = vec4((texturePos - 0.5) * 2, depth, 1.f);
    vec4 inversed = invProj * ndc; // going back from projected
    inversed /= inversed.w;
    return inversed.xyz;
}

in vec2 UV;

out vec4 outColor;

vec3 ScreenSpaceReflections(vec3 position, vec3 normal);

void main()
{

    vec3 position = generatePositionFromDepth(UV, texture(tDepth, UV).x);
    vec4 normal = view * vec4(texture(tNorm, UV).xyz, 0.0);
    float metallic = texture(tMetallic, UV).r;
    if(metallic < 0.1){
        outColor = texture(tFrame, UV);
    }else {
        vec3 reflectionDirection = normalize(reflect(position, normalize(normal.xyz)));
        outColor = vec4(ScreenSpaceReflections(position, normalize(reflectionDirection)), 1.0);
        if(outColor.xyz == vec3(0.f)){
            outColor = texture(tFrame, UV);
        }
    }

}

// Screen space reflections
vec3 ScreenSpaceReflections(vec3 position, vec3 reflection) {
    vec3 step = rayStep * reflection;
    vec3 newPosition = position + step;

    for(int i = 0; i < iterationCount; i++) {
        vec4 newViewPos = vec4(newPosition, 1.0);
        vec4 samplePosition = proj * newViewPos;
        samplePosition.xy = (samplePosition.xy / samplePosition.w) * 0.5 + 0.5;

        float currentDepth = abs(newViewPos.z);
        float sampledDepth = abs(generatePositionFromDepth(samplePosition.xy, texture(tDepth, samplePosition.xy).x).z);

        float delta = abs(currentDepth - sampledDepth);
        if(delta < 0.05f) {
            return texture(tFrame, samplePosition.xy).xyz;
        }

        step *= 1.0 - 0.5 * max(sign(currentDepth - sampledDepth), 0.0);
        newPosition += step * (sign(sampledDepth - currentDepth) + 0.000001);
    }
    return vec3(0.0);
}