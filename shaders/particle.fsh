#version 150

in vec2 UV;
out vec4 fragColor;

uniform sampler2D texture_sampler;

void main(void) {
    float alpha = 0.9 * texture(texture_sampler, UV).r;
    fragColor = vec4(0.8, 0.8, 0.8, alpha);
}

