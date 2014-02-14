

#include "ProceduralGenerator.h"
#include <glm/gtc/noise.hpp>
#include <math.h>
#include <iostream>

float perlin_noise(float x, float y, float persistance, int octaves) {
    float frequency = 1.0;
    float amplitude = 1.0;
    float sum = 0.0;

    for (int i = 0; i < octaves; i++) {
        sum += glm::simplex(glm::vec3(x, y, 0.5f) * frequency) * amplitude;
        amplitude *= persistance;
        frequency *= 2.0;
    }

    return sum;
}


float * grass_texture() {
    const float kernel[3][3] = {
        {1/16.0, 1/8.0, 1/16.0},
        {1/8.0, 1/4.0, 1/8.0},
        {1/16.0, 1/8.0, 1/16.0}
    };
    float * noise = (float *) malloc(sizeof(float) * 256 * 256);
    for (int x = 0; x < 256; x++) {
        for (int y = 0; y < 256; y++) {
            float sum = 0.0;
            // 3 x 3 gaussian blur
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j <= 3; j++) {
                    sum += perlin_noise((x + i) / 16.0f, (y + j) / 16.0f, 0.8, 3) * kernel[i][j];
                }
            }
            noise[x * 256 + y] =sum;
        }
    }
    return noise;
}

float * terrain_height() {
    return NULL;
}

float * bark_texture() {
    return NULL;
}

