#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 textures;

out vec4 normal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    mat4 mv = view * model;
    gl_Position = projection * mv * vec4(position, 1.0f);
    normal = model * vec4(normals, 1.0f);
    texCoord = textures;
}
