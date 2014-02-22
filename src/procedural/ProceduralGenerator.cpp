

#include "ProceduralGenerator.h"
#include <glm/gtc/noise.hpp>
#include <math.h>
#include <iostream>

// TODO: implement these ...
// function bias(b, value)
// {
//   return (value / ((((1.0/b) - 2.0)*(1.0 - value))+1.0));
// }

// function gain(g, value)
// {
//   if(value < 0.5) {
//     return bias(g, value * 2.0)/2.0;
//   } else {
//     return bias(1.0 - g, value * 2.0 - 1.0)/2.0 + 0.5;
//   }
// }

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

// makes everything between 0.0 and 1.0
void normalize_image(float * image, int width, int height) {
    float min = image[0];
    float max = image[0];
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            float el = image[i * width + j];
            if (min > el) {
                min = el;
            }
            if (max < el) {
                max = el;
            }
        }
    }
    float scale = max - min;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            image[i * width + j] = (image[i * width + j] - min) / scale;
        }
    }
}

// give given size, not desired size
float * apply_filter(float * image, int image_w, int image_h, float * filter, int filter_w, int filter_h) {
    int filtered_w = image_w - (filter_w / 2) * 2;
    int filtered_h = image_h - (filter_h / 2) * 2;

    float * filtered = (float *) malloc(sizeof(float) * filtered_w * filtered_h);

    for (int i = 0; i < filtered_w; i++) {
        for (int j = 0; j < filtered_h; j++) {
            float sum = 0.0f;
            for (int k = 0; k < filter_w; k++) {
                for (int l = 0; l < filter_h; l++) {
                    sum += filter[k * filter_w + l] * image[(i + k) * image_w + (j + l)];
                }
            }
            filtered[i * filtered_w + j] = sum;
        }
    }

    return filtered;        
}

void sqrt_img(float * image, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        image[i] = sqrt(image[i]);
    }       
}

void sqr_img(float * image, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        image[i] = image[i] * image[i];
    }       
}


float * grass_texture(int width, int height) {
    float * noise = (float *) malloc(sizeof(float) * width * height);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            noise[x * width + y] = perlin_noise((x + 0.3)/63.2f, (y + 0.7)/63.8f, 0.8, 8);
        }
    }
    normalize_image(noise, width, height);
    return noise;
}

float * terrain_height(int width, int height) {
    float * noise = (float *) malloc(sizeof(float) * width * height);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            noise[x * width + y] = perlin_noise((x + 0.3)/63.2f, (y + 0.7)/63.8f, 0.5, 10);
        }
    }
    normalize_image(noise, width, height);
    return noise;
}

float * bark_texture(int width, int height) {
    float * noise = (float *) malloc(sizeof(float) * width * height);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            noise[x * width + y] = perlin_noise((x + 0.3)/15.7f, (y + 0.7)/63.8f, 0.5, 10);
        }
    }
    normalize_image(noise, width, height);
    return noise;
}

float * bark_test(int width, int height) {
    return (float *) calloc(width * height, sizeof(float));
}
