
#include "WaterScene.h"
#include "ProceduralGenerator.h"
#include "lodepng.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <iostream>

void WaterScene::setup_scene() {
    parallax_shader = load_shaders("shaders/parallax.vsh", "", "shaders/parallax.fsh");
    water_shader = load_shaders("shaders/water.vsh", "", "shaders/water.fsh");
    particle_shader = load_shaders("shaders/particle.vsh", "shaders/particle.gsh", "shaders/particle.fsh");

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

    glm::vec3 eye = glm::vec3(-8,-2,5);
    glm::mat4 view_transform = glm::lookAt(eye, glm::vec3(0,1,0), glm::vec3(0,-1,0));

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
    glm::mat4 model_transform = glm::mat4();

    wall = Model::create_model(parallax_shader);
    wall->add_attribute(&vertices[0], sizeof(vertices), 3, "vertex_position");

    wall->add_attribute(&normals[0], sizeof(normals), 3, "vertex_normal");
    wall->add_attribute(&delta_u[0], sizeof(delta_u), 3, "delta_u");
    wall->add_attribute(&delta_v[0], sizeof(delta_v), 3, "delta_v");

    wall->add_attribute(&texture_coords[0], sizeof(texture_coords), 2, "vertex_UV");
    wall->add_indices(&indices[0], sizeof(indices));
    wall->add_2d_texture(&raw_texture[0], width, height, GL_UNSIGNED_BYTE, 4, "texture_sampler");
    wall->add_2d_texture(&raw_bump[0], width, height, GL_UNSIGNED_BYTE, 4, "bump_map");

    wall->add_uniform("view_transform", &view_transform[0][0], 16);
    wall->add_uniform("model_transform", &model_transform[0][0], 16);
    wall->add_uniform("eye", &eye[0], 3);


    int n_particles = 500;

    int * particle_indices = (int *) malloc(sizeof(int) * n_particles);
    float * time_offset = (float *) malloc(sizeof(float) * n_particles);
    float * velocities = (float *) malloc(sizeof(float) * 3 * n_particles);

    for (int i = 0; i < n_particles; i++) {
        particle_indices[i] = i;
        time_offset[i] = glm::gaussRand(0.0, 0.42);
        velocities[i * 3 + 2] = 3.0;
        velocities[i * 3 + 1] = glm::gaussRand(-1.0, 0.3);
        velocities[i * 3 + 0] = glm::gaussRand(0.0, 0.3);
    }

    float gaussian [] = {
        0.0020, 0.0043, 0.0084, 0.0147, 0.0229, 0.0319, 0.0399, 0.0446, 0.0446, 0.0399, 0.0319, 0.0229, 0.0147, 0.0084, 0.0043, 0.0020,
        0.0043, 0.0094, 0.0183, 0.0319, 0.0498, 0.0695, 0.0868, 0.0970, 0.0970, 0.0868, 0.0695, 0.0498, 0.0319, 0.0183, 0.0094, 0.0043,
        0.0084, 0.0183, 0.0357, 0.0622, 0.0970, 0.1353, 0.1690, 0.1889, 0.1889, 0.1690, 0.1353, 0.0970, 0.0622, 0.0357, 0.0183, 0.0084,
        0.0147, 0.0319, 0.0622, 0.1084, 0.1690, 0.2359, 0.2946, 0.3292, 0.3292, 0.2946, 0.2359, 0.1690, 0.1084, 0.0622, 0.0319, 0.0147,
        0.0229, 0.0498, 0.0970, 0.1690, 0.2636, 0.3679, 0.4594, 0.5134, 0.5134, 0.4594, 0.3679, 0.2636, 0.1690, 0.0970, 0.0498, 0.0229,
        0.0319, 0.0695, 0.1353, 0.2359, 0.3679, 0.5134, 0.6412, 0.7165, 0.7165, 0.6412, 0.5134, 0.3679, 0.2359, 0.1353, 0.0695, 0.0319,
        0.0399, 0.0868, 0.1690, 0.2946, 0.4594, 0.6412, 0.8007, 0.8948, 0.8948, 0.8007, 0.6412, 0.4594, 0.2946, 0.1690, 0.0868, 0.0399,
        0.0446, 0.0970, 0.1889, 0.3292, 0.5134, 0.7165, 0.8948, 1.0000, 1.0000, 0.8948, 0.7165, 0.5134, 0.3292, 0.1889, 0.0970, 0.0446,
        0.0446, 0.0970, 0.1889, 0.3292, 0.5134, 0.7165, 0.8948, 1.0000, 1.0000, 0.8948, 0.7165, 0.5134, 0.3292, 0.1889, 0.0970, 0.0446,
        0.0399, 0.0868, 0.1690, 0.2946, 0.4594, 0.6412, 0.8007, 0.8948, 0.8948, 0.8007, 0.6412, 0.4594, 0.2946, 0.1690, 0.0868, 0.0399,
        0.0319, 0.0695, 0.1353, 0.2359, 0.3679, 0.5134, 0.6412, 0.7165, 0.7165, 0.6412, 0.5134, 0.3679, 0.2359, 0.1353, 0.0695, 0.0319,
        0.0229, 0.0498, 0.0970, 0.1690, 0.2636, 0.3679, 0.4594, 0.5134, 0.5134, 0.4594, 0.3679, 0.2636, 0.1690, 0.0970, 0.0498, 0.0229,
        0.0147, 0.0319, 0.0622, 0.1084, 0.1690, 0.2359, 0.2946, 0.3292, 0.3292, 0.2946, 0.2359, 0.1690, 0.1084, 0.0622, 0.0319, 0.0147,
        0.0084, 0.0183, 0.0357, 0.0622, 0.0970, 0.1353, 0.1690, 0.1889, 0.1889, 0.1690, 0.1353, 0.0970, 0.0622, 0.0357, 0.0183, 0.0084,
        0.0043, 0.0094, 0.0183, 0.0319, 0.0498, 0.0695, 0.0868, 0.0970, 0.0970, 0.0868, 0.0695, 0.0498, 0.0319, 0.0183, 0.0094, 0.0043,
        0.0020, 0.0043, 0.0084, 0.0147, 0.0229, 0.0319, 0.0399, 0.0446, 0.0446, 0.0399, 0.0319, 0.0229, 0.0147, 0.0084, 0.0043, 0.0020
    };

    particles = Model::create_model(particle_shader);
    particles->add_attribute(velocities, sizeof(float) * 3 * n_particles, 3, "initial_velocity");
    particles->add_attribute(time_offset, sizeof(float) * n_particles, 1, "time_offset");
    particles->add_indices(particle_indices, sizeof(int) * n_particles);
    particles->set_draw_mode(GL_POINTS);
    particles->add_uniform("view_transform", &view_transform[0][0], 16);
    particles->add_uniform("model_transform", &model_transform[0][0], 16);
    particles->add_uniform("eye", &eye[0], 3);
    particles->add_2d_texture(&gaussian[0], 16, 16, GL_FLOAT, 1, "texture_sampler");

    free(particle_indices);
    free(time_offset);
    free(velocities);


    water = Model::create_model(water_shader);
    water->add_attribute(&vertices[0], sizeof(vertices), 3, "vertex_position");

    water->add_attribute(&normals[0], sizeof(normals), 3, "vertex_normal");
    water->add_attribute(&delta_u[0], sizeof(delta_u), 3, "delta_u");
    water->add_attribute(&delta_v[0], sizeof(delta_v), 3, "delta_v");

    water->add_attribute(&texture_coords[0], sizeof(texture_coords), 2, "vertex_UV");
    water->add_indices(&indices[0], sizeof(indices));
    water->add_2d_texture(&raw_bump[0], width, height, GL_UNSIGNED_BYTE, 4, "bump_map");

    model_transform = glm::rotate(glm::translate(glm::mat4(), glm::vec3(0, 2, 4)), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    water->add_uniform("view_transform", &view_transform[0][0], 16);
    water->add_uniform("model_transform", &model_transform[0][0], 16);
    water->add_uniform("eye", &eye[0], 3);
}


void WaterScene::render(int w, int h, float t) {
    glm::mat4 perspective_transform = glm::perspective(45.0f, ((float)w) / h, 0.1f, 1000.0f);

    glUseProgram(parallax_shader);
    wall->add_uniform("perspective_transform", &perspective_transform[0][0], 16);
    wall->render_model();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glUseProgram(water_shader);
    // water->add_uniform("perspective_transform", &perspective_transform[0][0], 16);
    // water->render_model();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(particle_shader);
    particles->add_uniform("perspective_transform", &perspective_transform[0][0], 16);
    particles->add_uniform("t", &t, 1);
    particles->render_model();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
