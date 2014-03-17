
#version 330 core

in vec3 vp;
in vec3 vn;
in vec3 c;
layout(location = 0) out vec4 positions;
layout(location = 1) out vec4 normals;
layout(location = 2) out vec4 colors;

void main() {
    positions = vec4(vp, 1.0);
    normals = vec4(vn, 0.0);
    colors = vec4(c, 1.0);
}
