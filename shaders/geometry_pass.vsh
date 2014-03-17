
#version 330 core

in vec3 vertex_position;
in vec3 vertex_normal;
uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 perspective_transform;
uniform mat3 normal_transform;
uniform vec3 color;
out vec3 vp;
out vec3 vn;
out vec3 c;

void main() {
    mat4 MV = view_transform * model_transform;
    vec4 world_coords = MV * vec4(vertex_position, 1.0);
    gl_Position = perspective_transform * world_coords;
    vp = world_coords.xyz;
    vn = vertex_normal;
    // vn = normalize(normal_transform * vertex_normal);
    c = color;
}
