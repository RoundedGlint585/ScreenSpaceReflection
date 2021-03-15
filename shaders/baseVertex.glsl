#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textures;
out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoords;
//out float gl_FragDepth;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D textureNormal;
void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    WorldPos =  (model * vec4(position, 1.f)).xyz; // (view * model * vec4(position, 1.f)).xyz;
    Normal = (model * texture(textureNormal, textures)).xyz;
    TexCoords = textures;
}