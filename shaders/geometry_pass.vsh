
#version 330 core

in vec3 vertex_position;
in vec3 vertex_normal;
uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 perspective_transform;
uniform mat3 normal_transform;
out vec3 vp;
out vec3 vn;
flat out int m;

void main() {
    mat4 MV = view_transform * model_transform;
    vec4 temp = MV * vec4(vertex_position, 1.0);
    gl_Position = perspective_transform * temp;
    vp = temp.xyz;
    vn = normalize(normal_transform * vertex_normal);
    m = 1;
}
