
#version 330 core

in vec3 vp;
in vec3 vn;
flat in int m;
layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 norm;
layout(location = 2) out int mat;

void main() {
    pos = vec3(1.0, 1.0, 0.0);//vp;
    norm = vec3(0.0, 1.0, 0.0);//normalize(vn);
    mat = 2;
}
