

#include "TreeScene.h"
#include "Model.h"
#include "IndexedFaceSet.h"
#include "TreeGenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void TreeScene::setup_scene() {
    Model * normal_tree_model = TreeGenerator::generate_normal_tree();
    glm::mat4 view_transform = glm::lookAt(glm::vec3(0,4,18), glm::vec3(0,3,0), glm::vec3(0,1,0));
    glm::mat4 model_transform = glm::translate(glm::mat4(), glm::vec3(-4.0, 0.0, 0.0));
    normal_tree_model->add_uniform_matrix("view_transform", &view_transform[0][0]);
    normal_tree_model->add_uniform_matrix("model_transform", &model_transform[0][0]);
    models.emplace(normal_tree_model->g_shader_program(), normal_tree_model);

    Model * willow_tree_model = TreeGenerator::generate_willow_tree();
    model_transform = glm::translate(glm::mat4(), glm::vec3(8.0, 0.0, 0.0)) * model_transform;
    willow_tree_model->add_uniform_matrix("view_transform", &view_transform[0][0]);
    willow_tree_model->add_uniform_matrix("model_transform", &model_transform[0][0]);
    models.emplace(willow_tree_model->g_shader_program(), willow_tree_model);

}

