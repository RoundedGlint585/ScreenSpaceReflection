#version 420 core

const vec2 quadVertices[4] = { vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0) };

out vec2 UV;

void main()
{
    gl_Position = vec4(quadVertices[gl_VertexID], 0.0, 1.0);
    UV = (gl_Position.xy+vec2(1,1))/2.0;
}
