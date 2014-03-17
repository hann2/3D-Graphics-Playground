
#include "ToonScene.h"
#include "Model.h"
#include "IndexedFaceSet.h"
#include "load_collada.h"
#include "TreeGenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void ToonScene::setup_scene() {
    geometry_pass_shader = load_shaders("shaders/geometry_pass.vsh", "", "shaders/geometry_pass.fsh");

    Model * suzanne_model = Model::create_model(geometry_pass_shader);
    IndexedFaceSet * suzanne_geometry = load_collada("assets/models/suzanne.dae");

    float * suzanne_vertices = suzanne_geometry->g_vertex_buffer();
    float * suzanne_normals = suzanne_geometry->g_vertex_normal_buffer();
    int * suzanne_indices = suzanne_geometry->g_index_buffer();
    int suzanne_vertices_size = suzanne_geometry->g_vertex_buffer_size();
    int suzanne_normals_size = suzanne_geometry->g_vertex_normal_buffer_size();
    int suzanne_indices_size = suzanne_geometry->g_index_buffer_size();
    suzanne_model->add_attribute(suzanne_vertices, suzanne_vertices_size, 3, "vertex_position");
    suzanne_model->add_attribute(suzanne_normals, suzanne_normals_size, 3, "vertex_normal");
    suzanne_model->add_indices(suzanne_indices, suzanne_indices_size);
    free(suzanne_vertices);
    free(suzanne_normals);
    free(suzanne_indices);
    delete suzanne_geometry;

    glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f);

    glm::mat4 view_transform = glm::lookAt(glm::vec3(0,4,10), glm::vec3(0,3,0), glm::vec3(0,1,0));
    glm::mat4 model_transform = glm::translate(glm::mat4(), glm::vec3(2.0, 2.0, -2.0));
    suzanne_model->add_uniform("view_transform", &view_transform[0][0], 16);
    suzanne_model->add_uniform("model_transform", &model_transform[0][0], 16);
    suzanne_model->add_uniform("color", &color[0], 3);
    models.emplace(geometry_pass_shader, suzanne_model);

    Model * normal_tree_model = TreeGenerator::generate_normal_tree(geometry_pass_shader);
    model_transform = glm::translate(glm::mat4(), glm::vec3(-2.0, 0.0, 0.0));
    color = glm::vec3(0.529, 0.333, 0.169);
    normal_tree_model->add_uniform("view_transform", &view_transform[0][0], 16);
    normal_tree_model->add_uniform("model_transform", &model_transform[0][0], 16);
    normal_tree_model->add_uniform("color", &color[0], 3);
    models.emplace(geometry_pass_shader, normal_tree_model);

    lighting_pass_shader = load_shaders("shaders/lighting_pass.vsh", "", "shaders/deferred_toon.fsh");
    billboard_model = Model::create_model(lighting_pass_shader);

    GLfloat billboard_vertices [] = {
        1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0
    };

    GLint billboard_indices [] = {
        0, 1, 2, 2, 3, 0
    };

    billboard_model->add_attribute(billboard_vertices, sizeof(billboard_vertices), 3, "vertex_position");
    billboard_model->add_indices(billboard_indices, sizeof(billboard_indices));

    glm::vec3 sun = glm::normalize(glm::vec3(0.0f, 1.0f, 8.0f));
    billboard_model->add_uniform("sun", &sun[0], 3);
    glm::vec3 camera = glm::normalize(glm::vec3(0.0f, 0.0f, 8.0f));
    billboard_model->add_uniform("camera", &camera[0], 3);
}

void ToonScene::setup_g_buffers() {
    add_g_buffer(GL_RGBA32F, GL_RGBA, GL_FLOAT, "positions");
    add_g_buffer(GL_RGBA16F, GL_RGBA, GL_FLOAT, "normals");
    add_g_buffer(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, "colors");
}

