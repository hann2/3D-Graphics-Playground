#version 330 core

uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 perspective_transform;
in vec3 vertex_position;
in vec2 vertex_UV;
in vec3 vertex_normal;
in vec3 delta_u;
in vec3 delta_v;

out vec2 UV;
out vec3 normal;
out vec3 du;
out vec3 dv;
out vec3 pos;

void main() {
    UV = vertex_UV;
    mat4 normal_transform = transpose(model_transform);
    normal = vec3(normal_transform * vec4(vertex_normal, 0));
    du = vec3(normal_transform * vec4(delta_u, 0));
    dv = vec3(normal_transform * vec4(delta_v, 0));
    vec4 temp = model_transform * vec4(vertex_position, 1.0);
    gl_Position = perspective_transform * view_transform * temp;
    pos = temp.xyz / temp.w;
}
