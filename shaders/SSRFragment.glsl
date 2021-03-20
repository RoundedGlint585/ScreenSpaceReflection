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
uniform float distanceBias = 0.05f;
uniform bool enableSSR = true;

vec3 generatePositionFromDepth(vec2 texturePos, float depth){
    vec4 ndc = vec4((texturePos - 0.5) * 2, depth, 1.f);
    vec4 inversed = invProj * ndc; // going back from projected
    inversed /= inversed.w;
    return inversed.xyz;
}

vec2 generateProjectedPosition(vec3 pos){
    vec4 newViewPos = vec4(pos, 1.0);
    vec4 samplePosition = proj * newViewPos;
    samplePosition.xy = (samplePosition.xy / samplePosition.w) * 0.5 + 0.5;
    return samplePosition.xy;
}

in vec2 UV;

out vec4 outColor;

vec3 ScreenSpaceReflections(vec3 position, vec3 normal);

void main()
{

    vec3 position = generatePositionFromDepth(UV, texture(tDepth, UV).x);
    vec4 normal = view * vec4(texture(tNorm, UV).xyz, 0.0);
    float metallic = texture(tMetallic, UV).r;
    if(!enableSSR || metallic < 0.1){
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
    vec3 newPos = position + step;
    for(int i = 0; i < iterationCount; i++) {
        vec2 samplePosition = generateProjectedPosition(newPos);
        float currentDepth = abs(newPos.z);
        float sampledDepth = abs(generatePositionFromDepth(samplePosition, texture(tDepth, samplePosition).x).z);

        float delta = abs(currentDepth - sampledDepth);
        if(delta < distanceBias) {
            return texture(tFrame, samplePosition).xyz;
        }
        float directionSign = sign(currentDepth - sampledDepth);
        //this is literally binary search
        step = step * (1.0 - 0.15 * max(directionSign, 0.0));
        newPos = newPos + step * (-directionSign);
    }
    return vec3(0.0);
}