
#include "ParallaxScene.h"
#include "Model.h"
#include "ProceduralGenerator.h"
#include "lodepng.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void ParallaxScene::setup_scene() {
    GLint grass_shader = load_shaders("shaders/parallax.vsh", "", "shaders/parallax.fsh");

    float vertices [] = { 
        -4.0f, -4.0f, 0.0f,
        4.0f,  -4.0f, 0.0f,
        4.0f,  4.0f,  0.0f,
        -4.0f, 4.0f,  0.0f
    };
    float normals [] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };
    float delta_u [] = {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };
    float delta_v [] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    float texture_coords [] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    int indices [] = {
        0, 1, 2, 2, 3, 0
    };

    glm::vec3 eye = glm::vec3(-5,0,4);
    glm::mat4 view_transform = glm::lookAt(eye, glm::vec3(0,0,0), glm::vec3(0,-1,0));

    float * grass = grass_texture(256, 256);

    std::vector<unsigned char> raw_bump;
    unsigned width, height;
    unsigned error = lodepng::decode(raw_bump, width, height, "assets/textures/lion-bump-smooth.png");
    if (error) {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }

    std::vector<unsigned char> raw_texture;
    error = lodepng::decode(raw_texture, width, height, "assets/textures/lion.png");
    if (error) {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }

    Model * model = Model::create_model(grass_shader);
    model->add_attribute(&vertices[0], sizeof(vertices), 3, "vertex_position");

    model->add_attribute(&normals[0], sizeof(normals), 3, "vertex_normal");
    model->add_attribute(&delta_u[0], sizeof(delta_u), 3, "delta_u");
    model->add_attribute(&delta_v[0], sizeof(delta_v), 3, "delta_v");

    model->add_attribute(&texture_coords[0], sizeof(texture_coords), 2, "vertex_UV");
    model->add_indices(&indices[0], sizeof(indices));
    model->add_2d_texture(&raw_texture[0], width, height, GL_UNSIGNED_BYTE, 4, "texture_sampler");
    model->add_2d_texture(&raw_bump[0], width, height, GL_UNSIGNED_BYTE, 4, "bump_map");

    glm::mat4 model_transform = glm::mat4();
    model->add_uniform("view_transform", &view_transform[0][0], 16);
    model->add_uniform("model_transform", &model_transform[0][0], 16);
    model->add_uniform("eye", &eye[0], 3);
    models.emplace(grass_shader, model);

    free(grass);
}
