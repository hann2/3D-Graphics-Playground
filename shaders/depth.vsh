#version 330 core

in vec3 vertex_position;

uniform mat4 mvp;

void main(){
    gl_Position =  mvp * vec4(vertex_position, 1);
}

