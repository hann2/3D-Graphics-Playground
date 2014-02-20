#version 330 core

in vec2 UV;
in float light;
out vec4 fragColor;
uniform sampler2D texture_sampler;
const vec3 GRASS_COLOR = vec3(20, 100, 20);
const vec3 DIRT_COLOR = vec3(255, 215, 0);

void main(void) {
    float I = texture(texture_sampler, UV).r;
    float l = light;
    if (l <= 0) {
        l = 1.0;
    }
    fragColor = vec4(l * ((1.0 - I) * GRASS_COLOR + I * DIRT_COLOR) / 255.0, 1.0);
}
