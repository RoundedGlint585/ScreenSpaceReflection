#version 330 core
layout(location = 0) out vec4 normalColor;
layout(location = 1) out vec4 depthColor;
layout(location = 2) out vec4 metallicColor;
in vec4 pos;
in vec4 normal;
in vec2 texCoord;
uniform sampler2D textureMetallic;
// material parameters


void main()
{
    normalColor = normal;
    depthColor = vec4(vec3(gl_FragCoord.z), 1.f);
    float metallic = texture(textureMetallic, texCoord).r;
    metallicColor = vec4(vec3(texture(textureMetallic, texCoord).r), 1.f);
}

