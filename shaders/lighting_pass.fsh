#version 330 core

out vec4 fragColor;
uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D materials;

void main() {
    vec3 pos = texture(positions, gl_FragCoord.xy).rgb;
    vec3 norm = texture(normals, gl_FragCoord.xy).rgb;
    float mat = texture(materials, gl_FragCoord.xy).r;
    float depth = pos.z;
    vec3 c = vec3(1.0, 0.0, 1.0);

    if (mat == 2) {
        c = norm;
    }

    if (depth == -1000.0) {
        c = vec3(0.0, 0.0, 1.0);
    }
    
    fragColor = vec4(c, 1.0);
}
