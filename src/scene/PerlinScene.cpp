
#include "PerlinScene.h"
#include "Model.h"
#include "ProceduralGenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void PerlinScene::setup_scene() {
    GLint topo_shader = load_shaders("shaders/textured.vsh", "", "shaders/topo.fsh");
    GLint grass_shader = load_shaders("shaders/textured.vsh", "", "shaders/textured.fsh");
    GLint no_shader = load_shaders("shaders/textured.vsh", "", "shaders/topo.fsh");
    float g_vertex_buffer_data[] = { 
        -1.0f, -1.0f, 0.0f,
        1.0f,  -1.0f, 0.0f,
        1.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  0.0f,
        -1.0f, 1.0f,  0.0f,
        -1.0f, -1.0f, 0.0f
    };
    float g_uv_buffer_data[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f
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
            model->add_attribute(&g_vertex_buffer_data[0], sizeof(g_vertex_buffer_data), 3, "vertex_position");
            model->add_attribute(&g_uv_buffer_data[0], sizeof(g_uv_buffer_data), 2, "vertex_UV");
            model->add_texture(texture_data[i][j], 256, 256, GL_FLOAT, 1, "texture_sampler");

            glm::mat4 model_transform = glm::translate(glm::mat4(), glm::vec3((i - 1) * 2.4f, (j - 1) * 2.4f, 0.0f));
            model->add_uniform_matrix("view_transform", &view_transform[0][0]);
            model->add_uniform_matrix("model_transform", &model_transform[0][0]);
            models.emplace(shaders[i][j], model);
        }
    }
}

