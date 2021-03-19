#version 330 core

uniform sampler2D tFrame;
uniform sampler2D tDepth;
uniform sampler2D tNorm;
uniform sampler2D tMetallic; // TODO: remove this
uniform mat4 proj;
uniform mat4 view;
uniform mat4 invProj;

uniform float rayStep;

vec3 getPosFromUV(vec2 UV, float depth)
{
    vec4 ndcPos;
    ndcPos.xy = UV * 2.0 - 1.0;
    ndcPos.z = depth; // sample from the gl_FragCoord.z image
    ndcPos.w = 1.0;

    vec4 clipPos = invProj * ndcPos;
    return (clipPos / clipPos.w).xyz;
}

in vec2 UV;

out vec4 outColor;

vec3 ScreenSpaceReflections(vec3 position, vec3 normal);

void main()
{

    vec3 position = getPosFromUV(UV, texture(tDepth, UV).x);
    vec4 normal = view * vec4(texture(tNorm, UV).xyz, 0.0);
    float metallic = texture(tMetallic, UV).r;
    if(metallic < 0.1){
        outColor =
        outColor = texture(tFrame, UV);
    }else {
        outColor = vec4(ScreenSpaceReflections(position, normalize(normal.xyz)), 1.0);
        if(outColor.xyz == vec3(0.f)){
            outColor = texture(tFrame, UV);
        }
    }

}

// Screen space reflections
vec3 ScreenSpaceReflections(vec3 position, vec3 normal)
{

    vec3 reflection = normalize(reflect(position, normal));

    float VdotR = max(dot(normalize(position), normalize(reflection)), 0.0);
    float fresnel = pow(VdotR, 5); // small hack, not fresnel at all

    vec3 step = rayStep * reflection;
    vec3 newPosition = position + step;

    float loops = max(sign(VdotR), 0.0) * 100;
    for(int i=0; i<loops ; i++)
    {
        vec4 newViewPos = vec4(newPosition, 1.0);
        vec4 samplePosition = proj * newViewPos;
        samplePosition.xy = (samplePosition.xy / samplePosition.w) * 0.5 + 0.5;

//        vec2 checkBoundsUV = max(sign(samplePosition.xy * (1.0 - samplePosition.xy)), 0.0);
//        if (checkBoundsUV.x * checkBoundsUV.y < 1.0){
//            step *= 0.5;
//            newPosition -= step;
//            continue;
//        }

        float currentDepth = abs(newViewPos.z);
        float sampledDepth = abs(getPosFromUV(samplePosition.xy, texture(tDepth, samplePosition.xy).x).z);

        float delta = abs(currentDepth - sampledDepth);
        if(delta < 0.05f)
        {
            return texture(tFrame, samplePosition.xy).xyz;
        }

        step *= 1.0 - 0.5 * max(sign(currentDepth - sampledDepth), 0.0);
        newPosition += step * (sign(sampledDepth - currentDepth) + 0.000001);
    }
    return vec3(0.0);
}