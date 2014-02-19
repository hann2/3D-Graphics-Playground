#version 330 core

uniform mat4 MVP;
in vec3 vertex_position;
in vec2 vertex_UV;
out vec2 UV;

void main() {
    gl_Position = MVP * vec4(vertex_position, 1.0);
    UV = vertex_UV;
}
