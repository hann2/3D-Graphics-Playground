#version 330 core

out vec4 fragColor;
uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D colors;
uniform mat4 inv_perspective;
uniform vec3 sun;
uniform vec2 direction_samples [16];
ivec2 pixel_coord;
vec3 pos;
vec3 norm;
vec3 col;
const float ambient_albedo = 0.3;
const float diffuse_albedo = 0.7;
// increasing R will make occlusion attenuate faster
const float R = 10;

void main() {
    pixel_coord = ivec2(gl_FragCoord.xy);
    pos = texelFetch(positions, pixel_coord, 0).xyz;
    norm = texelFetch(normals, pixel_coord, 0).xyz;
    col = texelFetch(colors, pixel_coord, 0).rgb;
    float depth = pos.z;
    vec3 c = vec3(0.0, 0.0, 0.0);

    // handle background
    if (depth == -1000.0f) {
        c = vec3(0.0, 0.0, 0.2);
    } else {
        float ambient_occlusion = 0.0f;
        for (int i = 0; i < 16; i++) {
            ivec2 pix = pixel_coord + ivec2(direction_samples[i]);
            vec3 vi = texelFetch(positions, pix, 0).xyz - pos;
            vec3 ni = texelFetch(normals, pix, 0).xyz - pos;
            ambient_occlusion += max(0, 1 - dot(vi, vi) / R * R) * max(0, dot(norm, vi) / length(vi));
        }
        float ambient_light = 1.0 - (ambient_occlusion / 6);
        float light = ambient_albedo * ambient_light + diffuse_albedo * dot(norm, sun);
        c = col * light;
    }
    fragColor = vec4(c, 1.0);
}
