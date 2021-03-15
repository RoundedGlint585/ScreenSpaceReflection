#version 330 core
out vec4 color;

uniform mat4 invView;
uniform mat4 proj;
uniform mat4 invProj;
uniform mat4 view;



uniform sampler2D tDepth;
uniform sampler2D tNorm;
uniform sampler2D tFrame;


uniform float strength;
uniform float metallic;
uniform float spec;

//noperspective in vec2 UV;
in vec2 UV;

const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const int numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;


#define Scale vec3(.8, .8, .8)
#define K 19.19


vec3 CalcViewPosition(in vec2 TexCoord)
{
    // Combine UV & depth into XY & Z (NDC)
    vec3 rawPosition                = vec3(TexCoord, texture(tDepth, TexCoord).r);

    // Convert from (0, 1) range to (-1, 1)
    vec4 ScreenSpacePosition        = vec4( rawPosition * 2 - 1, 1);

    // Undo Perspective transformation to bring into view space
    vec4 ViewPosition               = invProj * ScreenSpacePosition;

    // Perform perspective divide and return
    return                          ViewPosition.xyz / ViewPosition.w;
}




vec3 PositionFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(UV * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProj * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
    float depth;

    vec4 projectedCoord;

    for(int i = 0; i < numBinarySearchSteps; i++)
    {

        projectedCoord = proj * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = texture(tDepth, projectedCoord.xy).z;


        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if(dDepth > 0.0)
        hitCoord += dir;
        else
        hitCoord -= dir;
    }

    projectedCoord = proj * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

    return vec3(projectedCoord.xy, depth);
}

struct intersect{
    bool isInter;
    vec4 result;
};

intersect RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{

    dir *= step;


    float depth;
    int steps = 0;
    vec4 projectedCoord;


    for(int i = 0; i < maxSteps; i++)
    {
        hitCoord += dir;

        projectedCoord = proj * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = texture(tDepth, projectedCoord.xy).z;
        if(depth > 1000.0)
        continue;

        dDepth = hitCoord.z - depth;

        if((dir.z - dDepth) < 1.2)
        {
            if(dDepth <= 0.0)
            {
                vec4 Result;
                Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.f);

                return intersect(true, Result);
            }
        }

        steps++;
    }


    return intersect(false, vec4(projectedCoord.xy, depth, 0.f));
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 hash(vec3 a)
{
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx)*a.zyx);
}

void main(){

    vec3 viewNormal = vec3(texture(tNorm,UV) * invView);
    vec3 viewPos = CalcViewPosition(UV);
    vec3 albedo = texture(tFrame,UV).rgb;


    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo, metallic);
    vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);


    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));

    vec3 hitPos = viewPos;
    float dDepth;

    vec3 wp = vec3(vec4(viewPos, 1.0) * invView);
    vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), spec);
    intersect res = RayMarch((vec3(jitt) + reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);
    vec4 coords = res.result;



    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));

    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

    float ReflectionMultiplier = pow(metallic, reflectionSpecularFalloffExponent) *
    screenEdgefactor *
    -reflected.z;

    // Get color
    vec3 SSR = texture(tFrame, coords.xy).rgb  * Fresnel;

    //color = SSR;
    if(res.isInter){
        color = texture(tFrame, UV);
    }else {
        color = vec4(mix(texture(tFrame, UV).rgb, SSR, strength), 1.f); //replace strength with noise or another texture
    }
    //color = vec4(mix(texture(tFrame, UV).rgb, SSR, 0.6f), 1.f);
    //color = vec4(SSR, 1.f);
    //fragColor = mix(texture(baseImage, texCoord), vec4(skyColor, 1.0), reflectionStrength).rgb;

    //color = texture(tFrame, UV);
    //color = vec3(1.f);

}


//#version 330 core
//out vec4 FragColor;
//
//float maxDistance = 15;
//float resolution  = 0.3;
//int   steps       = 10;
//float thickness   = 0.5;
//
//uniform sampler2D positionTexture;
//uniform sampler2D normalTexture;
//uniform sampler2D depthTexture;
//uniform mat4 invProj;
//uniform vec2 viewPortSize;
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//vec3 PositionFromDepth(float depth, vec2 uvPos) {
//    float z = depth * 2.0 - 1.0;
//
//    vec4 clipSpacePosition = vec4(uvPos * 2.0 - 1.0, z, 1.0);
//    vec4 viewSpacePosition = invProj * clipSpacePosition;
//
//    // Perspective division
//    viewSpacePosition /= viewSpacePosition.w;
//
//    return viewSpacePosition.xyz;
//}
//
//float random (vec2 st) {
//    return fract(sin(dot(st.xy,
//    vec2(12.9898, 78.233)))*
//    43758.5453123);
//}
//
//vec3 getWorldPos(vec2 uv){
//    vec4 place = texture(positionTexture, uv);
//    place /= place.w;
//    return place.xyz;
//}
//
//vec2 createUVCoordFromWorld(vec3 worldPos){
//    vec4 projectedCoord = projection * vec4(worldPos, 1.f);
//    projectedCoord.xy /= projectedCoord.w;
//    projectedCoord.xy = (projectedCoord.xy * 0.5 + 0.5);
//    return projectedCoord.xy;
//}
//
//void main()
//{
//    // standart color calculation
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
//    FragColor = vec4(color, 1.0);// base color
//
//    // reflection calculation
//    vec2 currentPos = gl_FragCoord.xy/viewPortSize;
//    float depth =  texture(depthTexture, currentPos).x;// depthTexture rgb filled with one value(d,d,d)
//    vec3 worldPosition = PositionFromDepth(depth, currentPos);
//    //FragColor = vec4(PositionFromDepth(depth, currentPos), 1.f);
//    vec2 uvCoord = createUVCoordFromWorld(worldPosition);
//    FragColor = texture(positionTexture, uvCoord);
//
//    vec3 normal = normalize(texture(normalTexture, currentPos).xyz);
//    vec3 viewDir = normalize(worldPosition - cameraPos);
//    vec3 reflectionDir = normalize(reflect(viewDir, normal));
//
//    //ray marching
//    vec3 curRayPos = vec3(0);
//    vec2 curUV = vec2(0);
//    vec2 resultUV = vec2(0);
//    float curLength = 1;
//    bool isHitted = false;
//    for (int i = 0; i < 10; i++){
//        if (isHitted == false){
//            curRayPos = worldPosition + reflectionDir * curLength;
//            curUV = createUVCoordFromWorld(curRayPos);
//            float curDepth = texture(depthTexture, curUV.xy).x;
//            for (int j = 0; j < 16; j++){
//                float dDepth = curRayPos.z - curDepth;
//                if (abs(dDepth - curDepth) < 1.2){
//                    resultUV.xy = curUV.xy;
//                    isHitted = true;
//                    break;
//                }
//                curDepth = texture(depthTexture, curUV.xy + (random(vec2(j)) * 2)).x;
//            }
//        }
//    }
//    if(isHitted){
//        FragColor = texture(positionTexture, curUV.xy);
//    }else{
//        FragColor = vec4(0.f);
//    }
//    //FragColor = texture(positionTexture, curUV.xy);
//    //   FragColor = vec4(curUV, 1.f);
//}
//
