#version 330 core

out vec4 fragColor;
uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D colors;
uniform vec3 sun;
uniform vec3 camera;
const int cel_levels = 4;
vec3 pos;
vec3 norm;
vec3 col;

vec3 laplacian(sampler2D t, ivec2 c) {
    return (4 * texelFetch(t, c, 0) -
        (texelFetch(t, c + ivec2(1, 0), 0) +
        texelFetch(t, c + ivec2(-1, 0), 0) +
        texelFetch(t, c + ivec2(0, 1), 0) +
        texelFetch(t, c + ivec2(0, -1), 0))).xyz;
}

bool line_check(ivec2 coord) {
    float delta_z = abs(laplacian(positions, coord).z);
    // float delta_pos = length(laplacian(positions, coord));
    norm = texelFetch(normals, coord, 0).xyz;
    float delta_norm = abs(dot(norm, normalize(camera - pos)));
    if (delta_z > 0.1f || delta_norm < 0.3f) {
        return true;
    }
    return false;
}

bool is_line(ivec2 coord, int thickness) {
    int offs = (thickness - 1) / 2;
    int count = 0;
    for (int i = -offs; i <= offs; i++) {
        for (int j = -offs; j <= offs; j++) {
            if (line_check(coord + ivec2(i, j))) {
                count++;
            }
        }
    }
    if (count > offs) {
        return true;
    }
    return false;
}

void main() {
    ivec2 pixel_coord = ivec2(gl_FragCoord.xy);
    pos = texelFetch(positions, pixel_coord, 0).xyz;
    norm = texelFetch(normals, pixel_coord, 0).xyz;
    col = texelFetch(colors, pixel_coord, 0).rgb;
    float depth = pos.z;
    vec3 c = vec3(0.0, 0.0, 0.0);

    // handle background
    if (depth == -1000.0f) {
        c = vec3(0.0, 0.0, 0.2);
    } else {
        if (is_line(pixel_coord, 5)) {
            c = vec3(0.0, 0.0, 0.0);
        } else {
            int level = int((0.3 + 0.7 * dot(norm, sun)) * cel_levels);
            c = col * level / cel_levels;
        }
    }


    fragColor = vec4(c, 1.0);
}
