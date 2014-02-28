
#version 330 core

in vec3 vp;
in vec3 vn;
flat in int m;
out vec4 fragData[3];
void main() {
    fragData[0].rgb = vec3(1.0, 1.0, 0.0);//vp;
    fragData[1].rgb = vec3(0.0, 1.0, 0.0);//normalize(vn);
    fragData[2].r = 2;
}
