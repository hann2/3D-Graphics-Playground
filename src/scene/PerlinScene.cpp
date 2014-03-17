
#include "PerlinScene.h"
#include "Model.h"
#include "ProceduralGenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void PerlinScene::setup_scene() {
    GLint topo_shader = load_shaders("shaders/textured.vsh", "", "shaders/topo.fsh");
    GLint grass_shader = load_shaders("shaders/textured.vsh", "", "shaders/textured.fsh");
    GLint no_shader = load_shaders("shaders/textured.vsh", "", "shaders/topo.fsh");
    float vertices[] = { 
        -1.0f, -1.0f, 0.0f,
        1.0f,  -1.0f, 0.0f,
        1.0f,  1.0f,  0.0f,
        -1.0f, 1.0f,  0.0f
    };
    float texture_coords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    int indices[] = {
        0, 1, 2, 2, 3, 0
    };

    glm::mat4 view_transform = glm::lookAt(glm::vec3(4,4,10), glm::vec3(0,0,0), glm::vec3(0,1,0));

    float * empty = (float *) calloc(256 * 256, sizeof(float));
    float * texture_data [][3] = {
        {empty,                   empty, bark_test(256, 256)},
        {empty, grass_texture(256, 256), bark_texture(256, 256)},
        {empty,                   empty, terrain_height(256, 256)}
    };
    GLint shaders [][3] = {
        {no_shader,    no_shader, no_shader},
        {no_shader, grass_shader, no_shader},
        {no_shader,    no_shader, topo_shader}
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Model * model = Model::create_model(shaders[i][j]);
            model->add_attribute(&vertices[0], sizeof(vertices), 3, "vertex_position");
            model->add_attribute(&texture_coords[0], sizeof(texture_coords), 2, "vertex_UV");
            model->add_indices(&indices[0], sizeof(indices));
            model->add_2d_texture(texture_data[i][j], 256, 256, GL_FLOAT, 1, "texture_sampler");

            glm::mat4 model_transform = glm::translate(glm::mat4(), glm::vec3((i - 1) * 2.4f, (j - 1) * 2.4f, 0.0f));
            model->add_uniform("view_transform", &view_transform[0][0], 16);
            model->add_uniform("model_transform", &model_transform[0][0], 16);
            models.emplace(shaders[i][j], model);
        }
    }
}

