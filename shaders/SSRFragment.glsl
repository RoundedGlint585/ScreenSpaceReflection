#version 420 core
out vec4 color;

uniform mat4 invView;
uniform mat4 proj;
uniform mat4 invProj;
uniform mat4 view;

uniform sampler2D tDepth;
uniform sampler2D tNorm;
uniform sampler2D tFrame;
uniform sampler2D tMetallic;

uniform float strength = 0.5f;
uniform float spec;

vec2 UV = (gl_FragCoord.xy) / vec2(800.f, 600.f);

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
    vec4 ScreenSpacePosition        = vec4(rawPosition * 2 - 1, 1);

    // Undo Perspective transformation to bring into view space
    vec4 ViewPosition               = invProj * ScreenSpacePosition;

    // Perform perspective divide and return
    return ViewPosition.xyz / ViewPosition.w;
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

    for (int i = 0; i < numBinarySearchSteps; i++)
    {

        projectedCoord = proj * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = texture(tDepth, projectedCoord.xy).z;


        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if (dDepth > 0.0)
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

vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{

    dir *= step;


    float depth;
    int steps = 0;
    vec4 projectedCoord;


    for (int i = 0; i < maxSteps; i++)
    {
        hitCoord += dir;

        projectedCoord = proj * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = texture(tDepth, projectedCoord.xy).z;
        if (depth > 1000.0)
        continue;

        dDepth = hitCoord.z - depth;

        if ((dir.z - dDepth) < 1.2)
        {
            if (dDepth <= 0.0)
            {
                vec4 Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

                return Result;
            }
        }

        steps++;
    }


    return vec4(projectedCoord.xy, depth, 0.0);
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
    float metallic = texture(tMetallic, UV).r;
    if (metallic < 1){
        color = texture(tFrame, UV);
        return;
    }
    vec3 viewNormal = vec3(texture(tNorm, UV) * invView);
    vec3 viewPos = CalcViewPosition(UV);
    vec3 albedo = texture(tFrame, UV).rgb;


    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo, metallic);
    vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);


    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));

    vec3 hitPos = viewPos;
    float dDepth;

    vec3 wp = vec3(vec4(viewPos, 1.0) * invView);
    vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), spec);
    vec4 coords = RayMarch((vec3(jitt) + reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);


    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));


    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

    // Get color
    vec3 SSR = mix(texture(tFrame, UV).rgb, texture(tFrame, coords.xy).rgb * Fresnel, metallic);
    color = vec4(SSR, 1.f);
    if (SSR == vec3(0.2f)){
        color = texture(tFrame, UV);
    }

}






