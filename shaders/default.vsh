
#version 330 core

in vec3 vertex_position;
uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 perspective_transform;

void main() {
    mat4 MVP = perspective_transform * view_transform * model_transform;
    gl_Position = MVP * vec4(vertex_position, 1.0);
}
