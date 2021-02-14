#version 330
out vec4 color;

in vec3 normal;
in vec3 pos;
in vec2 texture;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float threshold;


void main()
{
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = lightPos - pos;
    float diff = max(dot(normalize(normal), normalize(lightDir)), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient+diffuse) * vec3(0.6f, 0.6f, 0.6f);
    color = vec4(result, 1.f);
}
//