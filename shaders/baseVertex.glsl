#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textures;
out vec3 pos;
out vec3 normal;
out vec2 texture;
//out float gl_FragDepth;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    pos = position;
    normal = normals;
    texture = textures;
}
//