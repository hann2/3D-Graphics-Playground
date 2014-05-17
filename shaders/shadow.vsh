#version 330 core

in vec3 vertex_position;
in vec3 vertex_normal;

out vec4 shadow_coord;
out vec3 fragment_normal;

uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 perspective_transform;
uniform mat4 light_mvp;


void main() {
    mat4 mvp = perspective_transform * view_transform * model_transform;
    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  mvp * vec4(vertex_position,1);
    
    shadow_coord = light_mvp * vec4(vertex_position,1);
    
    // should probably use normal matrix
    fragment_normal = vertex_normal;
}

