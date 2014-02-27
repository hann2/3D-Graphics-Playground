
#version 330 core

in vec3 vertex_position;
uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 perspective_transform;
uniform sampler1D tree_positioning;
#define M_PI 3.141592653

float access(int i) {
    return texelFetch(tree_positioning, gl_InstanceID + 100 * i, 0).r;
}

void main() {
    if (access(1) == 0) {
        gl_Position = perspective_transform * view_transform * model_transform * vec4(vertex_position, 1.0);
    } else {
        float x = (gl_InstanceID / 10) * 6.0 + 2.0;
        float y = access(2) * 20;
        float z = (mod(gl_InstanceID, 10)) * 6.0 + 2.0;
        vec4 trans = vec4(x, y, z, 0);

        float scale = access(1);
        float theta = access(0) * 2 * M_PI;
        mat4 rotation = mat4(
            cos(theta), 0.0, sin(theta), 0.0,
                   0.0, 1.0,        0.0, 0.0,
           -sin(theta), 0.0, cos(theta), 0.0,
                   0.0, 0.0,        0.0, 1.0);

        vec4 world_coordinate = rotation * model_transform * vec4(vertex_position * scale, 1.0) + trans;
        gl_Position = perspective_transform * view_transform * world_coordinate;
    }
}
