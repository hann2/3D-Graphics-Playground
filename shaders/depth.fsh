#version 330 core

// Ouput data
out vec3 fragment_depth;


void main() {
    fragment_depth = gl_FragCoord.z * vec3(1,1,1);
}
