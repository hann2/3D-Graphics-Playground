#version 330 core

in vec2 UV;
out vec4 fragColor;
uniform sampler2D texture_sampler;
const vec3 WATER_COLOR = vec3(35, 35, 122);
const vec3 BEACH_COLOR = vec3(238, 221, 130);
const vec3 GRASS_COLOR = vec3(20, 100, 20);
const vec3 DIRT_COLOR = vec3(120, 72, 0);
const vec3 MOUNTAIN_COLOR = vec3(139, 131, 120);
const vec3 SNOW_COLOR = vec3(255, 250, 250);

void main(void) {
    float I = texture(texture_sampler, UV).r;
    vec3 color;
    if (I < 1 / 6.0) {
        color = WATER_COLOR;
    } else if (I < 2 / 6.0) {
        color = BEACH_COLOR;
    } else if (I < 3 / 6.0) {
        color = GRASS_COLOR;
    } else if (I < 4 / 6.0) {
        color = DIRT_COLOR;
    } else if (I < 5 / 6.0) {
        color = MOUNTAIN_COLOR;
    } else {
        color = SNOW_COLOR;
    }
    fragColor = vec4(color / 255.0, 1.0);
}
