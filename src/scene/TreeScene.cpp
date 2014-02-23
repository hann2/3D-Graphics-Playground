

#include "TreeScene.h"
#include "Model.h"
#include "IndexedFaceSet.h"
#include "TreeGenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void TreeScene::setup_scene() {
    GLint wire_shader = load_shaders("shaders/default.vsh", "shaders/wire.gsh", "shaders/wire.fsh");

    Model * normal_tree_model = Model::create_model(wire_shader);
    IndexedFaceSet * normal_tree_geometry = TreeGenerator::generate_normal_tree();

    float * normal_tree_vertices = normal_tree_geometry->g_vertex_buffer();
    int * normal_tree_indices = normal_tree_geometry->g_index_buffer();
    int normal_tree_vertices_size = normal_tree_geometry->g_vertex_buffer_size();
    int normal_tree_indices_size = normal_tree_geometry->g_index_buffer_size();
    normal_tree_model->add_attribute(normal_tree_vertices, normal_tree_vertices_size, 3, "vertex_position");
    normal_tree_model->add_indices(normal_tree_indices, normal_tree_indices_size);

    glm::mat4 view_transform = glm::lookAt(glm::vec3(0,4,18), glm::vec3(0,3,0), glm::vec3(0,1,0));
    glm::mat4 model_transform = glm::translate(glm::mat4(), glm::vec3(-4.0, 0.0, 0.0)) *
        glm::rotate(glm::mat4(), -90.0f, glm::vec3(1.0, 0.0, 0.0));
    normal_tree_model->add_uniform_matrix("view_transform", &view_transform[0][0]);
    normal_tree_model->add_uniform_matrix("model_transform", &model_transform[0][0]);
    models.emplace(wire_shader, normal_tree_model);

    free(normal_tree_vertices);
    free(normal_tree_indices);
    delete normal_tree_geometry;

    Model * willow_tree_model = Model::create_model(wire_shader);
    IndexedFaceSet * willow_tree_geometry = TreeGenerator::generate_willow_tree();

    float * willow_tree_vertices = willow_tree_geometry->g_vertex_buffer();
    int * willow_tree_indices = willow_tree_geometry->g_index_buffer();
    int willow_tree_vertices_size = willow_tree_geometry->g_vertex_buffer_size();
    int willow_tree_indices_size = willow_tree_geometry->g_index_buffer_size();
    willow_tree_model->add_attribute(willow_tree_vertices, willow_tree_vertices_size, 3, "vertex_position");
    willow_tree_model->add_indices(willow_tree_indices, willow_tree_indices_size);

    model_transform = glm::translate(glm::mat4(), glm::vec3(8.0, 0.0, 0.0)) * model_transform;
    willow_tree_model->add_uniform_matrix("view_transform", &view_transform[0][0]);
    willow_tree_model->add_uniform_matrix("model_transform", &model_transform[0][0]);
    models.emplace(wire_shader, willow_tree_model);

    free(willow_tree_vertices);
    free(willow_tree_indices);
    delete willow_tree_geometry;
}

