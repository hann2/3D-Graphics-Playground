#version 150
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

// Vec3 containing the distances from a vertex to opposite edge
noperspective out vec3 dist;  

void main() {
    vec3 ab = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec3 ac = vec3(gl_in[0].gl_Position - gl_in[2].gl_Position);
    vec3 ba = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);
    vec3 bc = vec3(gl_in[1].gl_Position - gl_in[2].gl_Position);

    dist = vec3(length(cross(ab, ac)) / length(bc), 0, 0);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    dist = vec3(0, length(cross(ba, bc)) / length(ac),0);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    dist = vec3(0, 0, length(cross(ac, bc)) / length(ab));
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
} 