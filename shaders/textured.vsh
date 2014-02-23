#version 330 core

uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 perspective_transform;
in vec3 vertex_position;
in vec2 vertex_UV;
out vec2 UV;

void main() {
    mat4 MVP = perspective_transform * view_transform * model_transform;
    gl_Position = MVP * vec4(vertex_position, 1.0);
    UV = vertex_UV;
}
