#version 330 core

in vec2 UV;
out vec4 outColor;

uniform sampler2D textureFrame;
uniform sampler2D textureDepth;
uniform sampler2D textureNorm;
uniform sampler2D textureMetallic;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 invProj;

uniform float rayStep = 0.2f;
uniform int iterationCount = 100;
uniform float distanceBias = 0.05f;
uniform bool enableSSR = false;

vec3 generatePositionFromDepth(vec2 texturePos, float depth){
    vec4 ndc = vec4((texturePos - 0.5) * 2, depth, 1.f);
    vec4 inversed = invProj * ndc; // going back from projected
    inversed /= inversed.w;
    return inversed.xyz;
}

vec2 generateProjectedPosition(vec3 pos){
    vec4 samplePosition = proj * vec4(pos, 1.f);
    samplePosition.xy = (samplePosition.xy / samplePosition.w) * 0.5 + 0.5;
    return samplePosition.xy;
}

vec3 ScreenSpaceReflections(vec3 position, vec3 reflection) {
    vec3 step = rayStep * reflection;
    vec3 newPos = position + step;
    for(int i = 0; i < iterationCount; i++) {
        vec2 samplePosition = generateProjectedPosition(newPos);
        float currentDepth = abs(newPos.z);
        float sampledDepth = abs(generatePositionFromDepth(samplePosition, texture(textureDepth, samplePosition).x).z);

        float delta = abs(currentDepth - sampledDepth);
        if(delta < distanceBias) {
            return texture(textureFrame, samplePosition).xyz;
        }
        float directionSign = sign(currentDepth - sampledDepth);
        //this is sort of adapting step, should prevent lining reflection
        step = step * (1.0 - rayStep * max(directionSign, 0.0));
        newPos = newPos + step * (-directionSign);
    }
    return vec3(0.0);
}

void main(){
    vec3 position = generatePositionFromDepth(UV, texture(textureDepth, UV).x);
    vec4 normal = view * vec4(texture(textureNorm, UV).xyz, 0.0);
    float metallic = texture(textureMetallic, UV).r;
    if(!enableSSR || metallic < 0.05){
        outColor = texture(textureFrame, UV);
    }else {
        vec3 reflectionDirection = normalize(reflect(position, normalize(normal.xyz)));
        outColor = vec4(ScreenSpaceReflections(position, normalize(reflectionDirection)), 1.0);
        if(outColor.xyz == vec3(0.f)){
            outColor = texture(textureFrame, UV);
        }
    }
}
