
#include "OutlineScene.h"
#include "load_collada.h"
#include "IndexedFaceSet.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

OutlineScene::OutlineScene(int w, int h) {
    window_width = w;
    window_height = h;
    g_buffers = std::vector<g_buffer_t>();
    billboard_model = NULL;
}

void OutlineScene::setup_scene() {
    geometry_pass_shader = load_shaders("shaders/geometry_pass.vsh", "", "shaders/geometry_pass.fsh");
    lighting_pass_shader = load_shaders("shaders/lighting_pass.vsh", "", "shaders/lighting_pass.fsh");

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    add_g_buffer(3, GL_FLOAT, "positions");
    add_g_buffer(3, GL_FLOAT, "normals");
    add_g_buffer(1, GL_UNSIGNED_BYTE, "materials");

    GLuint rb;
    glGenRenderbuffers(1, &rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

    Model * suzanne_model = Model::create_model(geometry_pass_shader);
    IndexedFaceSet * suzanne_geometry = load_collada("assets/models/suzanne.dae");

    float * suzanne_vertices = suzanne_geometry->g_vertex_buffer();
    int * suzanne_indices = suzanne_geometry->g_index_buffer();
    suzanne_model->add_attribute(suzanne_vertices, suzanne_geometry->g_vertex_buffer_size(), 3, "vertex_position");
    suzanne_model->add_indices(suzanne_indices, suzanne_geometry->g_index_buffer_size());

    glm::mat4 perspective_transform = glm::perspective(45.0f, ((float)window_width) / window_height, 0.1f, 1000.0f);
    glm::mat4 view_transform = glm::lookAt(glm::vec3(1,3,8), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 model_transform = glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, 0.0));
    suzanne_model->add_uniform_matrix("perspective_transform", &perspective_transform[0][0]);
    suzanne_model->add_uniform_matrix("view_transform", &view_transform[0][0]);
    suzanne_model->add_uniform_matrix("model_transform", &model_transform[0][0]);
    
    models.push_back(suzanne_model);


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

}

void OutlineScene::add_g_buffer(int channels, GLenum type, std::string attribute_name) {

    GLint light_pass_uniform_id = glGetUniformLocation(lighting_pass_shader, attribute_name.c_str());
    if (light_pass_uniform_id == -1) {
        std::cout << "could not find attribute " << attribute_name << " in lighting pass.\n";
        return;
    }

    static const GLenum formats [] = {
        GL_RED, GL_RG, GL_RGB, GL_RGBA
    };
    GLenum format = formats[channels - 1];

    GLuint g_buffer_id;
    glGenTextures(1, &g_buffer_id);
    glBindTexture(GL_TEXTURE_RECTANGLE, g_buffer_id);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, format, window_width, window_height, 0, format, type, 0);

    int ind = g_buffers.size();
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + ind, GL_TEXTURE_RECTANGLE, g_buffer_id, 0);

    g_buffer_t g_buffer = {
        .ind = ind,
        .g_buffer_id = g_buffer_id,
        .light_pass_uniform_id = light_pass_uniform_id
    };

    g_buffers.push_back(g_buffer);
}

void OutlineScene::render(int w, int h) {
    const GLenum buffers [] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT0 + 1,
        GL_COLOR_ATTACHMENT0 + 2
    };

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Something wrong with FBO\n";
    }
    glDrawBuffers(3, buffers);
    glClearColor(0,0,-100,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glUseProgram(geometry_pass_shader);

    //render all models
    for (Model * model : models) {
        model->render_model();
    }

    //LIGHTING PASS
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(lighting_pass_shader);

    //bind all our g_buffers again
    for (g_buffer_t g_buffer : g_buffers) {
        glActiveTexture(GL_TEXTURE0 + g_buffer.ind);
        glBindTexture(GL_TEXTURE_RECTANGLE, g_buffer.g_buffer_id);
        glUniform1i(g_buffer.light_pass_uniform_id, g_buffer.ind);
    }

    billboard_model->render_model();
    glEnable(GL_DEPTH_TEST);
}

