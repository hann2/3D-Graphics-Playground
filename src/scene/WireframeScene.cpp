
#include "WireframeScene.h"
#include "Model.h"
#include "IndexedFaceSet.h"
#include "load_collada.h"
#include "load_cylinder.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void WireframeScene::setup_scene() {
    GLuint wire_shader = load_shaders("shaders/default.vsh", "shaders/wire.gsh", "shaders/wire.fsh");

    Model * suzanne_model = Model::create_model(wire_shader);
    IndexedFaceSet * suzanne_geometry = load_collada("assets/models/suzanne.dae");

    float * suzanne_vertices = suzanne_geometry->g_vertex_buffer();
    int * suzanne_indices = suzanne_geometry->g_index_buffer();
    int suzanne_vertices_size = suzanne_geometry->g_vertex_buffer_size();
    int suzanne_indices_size = suzanne_geometry->g_index_buffer_size();
    suzanne_model->add_attribute(suzanne_vertices, suzanne_vertices_size, 3, "vertex_position");
    suzanne_model->add_indices(suzanne_indices, suzanne_indices_size);

    glm::mat4 view_transform = glm::lookAt(glm::vec3(1,3,8), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 model_transform = glm::translate(glm::mat4(), glm::vec3(-2.0, 0.0, 0.0));
    suzanne_model->add_uniform_matrix("view_transform", &view_transform[0][0]);
    suzanne_model->add_uniform_matrix("model_transform", &model_transform[0][0]);
    models.emplace(wire_shader, suzanne_model);

    free(suzanne_vertices);
    free(suzanne_indices);
    delete suzanne_geometry;


    Model * cylinder_model = Model::create_model(wire_shader);
    IndexedFaceSet * cylinder_geometry = load_cylinder(32);

    float * cylinder_vertices = cylinder_geometry->g_vertex_buffer();
    int * cylinder_indices = cylinder_geometry->g_index_buffer();
    int cylinder_vertices_size = cylinder_geometry->g_vertex_buffer_size();
    int cylinder_indices_size = cylinder_geometry->g_index_buffer_size();
    cylinder_model->add_attribute(cylinder_vertices, cylinder_vertices_size, 3, "vertex_position");
    cylinder_model->add_indices(cylinder_indices, cylinder_indices_size);

    model_transform = glm::translate(glm::mat4(), glm::vec3(2.0, 0.0, 0.0));
    cylinder_model->add_uniform_matrix("view_transform", &view_transform[0][0]);
    cylinder_model->add_uniform_matrix("model_transform", &model_transform[0][0]);
    models.emplace(wire_shader, cylinder_model);

    free(cylinder_vertices);
    free(cylinder_indices);
    delete cylinder_geometry;
}
