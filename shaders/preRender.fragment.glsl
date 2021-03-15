#version 330 core
layout(location = 0) out vec4 posColor;
layout(location = 1) out vec4 normalColor;
layout(location = 2) out vec4 depthColor;
in vec4 pos;
in vec4 normal;

// material parameters


void main()
{
    posColor = pos;
    normalColor = normal;
    depthColor = vec4(vec3(gl_FragCoord.z), 1.f);
}

