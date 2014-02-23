
#include "WireframeScene.h"
#include "Model.h"
#include "IndexedFaceSet.h"
#include "load_collada.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void WireframeScene::setup_scene() {
    GLuint wire_shader = load_shaders("shaders/default.vsh", "shaders/wire.gsh", "shaders/wire.fsh");

    Model * suzanne_model = Model::create_model(wire_shader);
    IndexedFaceSet * suzanne_geometry = load_collada("assets/models/suzanne.dae");
    float * suzanne_mesh = suzanne_geometry->g_model_buffer();
    suzanne_model->add_attribute(suzanne_mesh, suzanne_geometry->g_model_buffer_size(), 3, "vertex_position");

    glm::mat4 view_transform = glm::lookAt(glm::vec3(1,3,8), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 model_transform = glm::mat4();
    suzanne_model->add_uniform_matrix("view_transform", &view_transform[0][0]);
    suzanne_model->add_uniform_matrix("model_transform", &model_transform[0][0]);
    models.emplace(wire_shader, suzanne_model);

    free(suzanne_mesh);
    delete suzanne_geometry;
}
