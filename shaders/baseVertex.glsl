#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textures;
out vec3 worldPos;
out vec3 worldNormal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D textureNormal;
void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    worldPos =  (model * vec4(position, 1.f)).xyz; // (view * model * vec4(position, 1.f)).xyz;
    worldNormal = (model * texture(textureNormal, textures)).xyz;
    texCoord = textures;
}