#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 velocity[];
in vec3 pos[];

uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 perspective_transform;
uniform vec3 eye;

out vec2 UV;

void main() {
    mat4 MVP = perspective_transform * view_transform * model_transform;
    vec3 eye_dir = normalize(eye - pos[0]);

    vec3 u = normalize(cross(velocity[0], eye_dir));
    vec3 b = normalize(velocity[0]);

    float hw = 0.015;
    float hh = length(velocity[0]) * 0.015;

    UV = vec2(0, 1);
    gl_Position = MVP * vec4(pos[0] - u * hw + b * hh, 1);
    EmitVertex();

    UV = vec2(0, 0);
    gl_Position = MVP * vec4(pos[0] - u * hw - b * hh, 1);
    EmitVertex();

    UV = vec2(1, 1);
    gl_Position = MVP * vec4(pos[0] + u * hw + b * hh, 1);
    EmitVertex();

    UV = vec2(1, 0);
    gl_Position = MVP * vec4(pos[0] + u * hw - b * hh, 1);
    EmitVertex();
}

