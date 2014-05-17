

#include "ShadowScene.h"
#include "Model.h"
#include "IndexedFaceSet.h"
#include "load_collada.h"
#include "TreeGenerator.h"
#include "ProceduralGenerator.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>

void ShadowScene::setup_scene() {
    GLfloat floor_vertices [] = {
        20.0, 0.0, 20.0,
        -20.0, 0.0, 20.0,
        -20.0, 0.0, -20.0,
        20.0, 0.0, -20.0
    };

    GLfloat floor_normals [] = {
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0
    };

    GLint floor_indices [] = {
        0, 2, 1, 2, 0, 3
    };

    IndexedFaceSet * suzanne_geometry = load_collada("assets/models/suzanne.dae");

    float * suzanne_vertices = suzanne_geometry->g_vertex_buffer();
    float * suzanne_normals = suzanne_geometry->g_vertex_normal_buffer();
    int * suzanne_indices = suzanne_geometry->g_index_buffer();
    int suzanne_vertices_size = suzanne_geometry->g_vertex_buffer_size();
    int suzanne_normals_size = suzanne_geometry->g_vertex_normal_buffer_size();
    int suzanne_indices_size = suzanne_geometry->g_index_buffer_size();

    GLuint depth_shader = load_shaders("shaders/depth.vsh", "", "shaders/depth.fsh");

    Model * suzanne_depth_model = Model::create_model(depth_shader);
    suzanne_depth_model->add_attribute(suzanne_vertices, suzanne_vertices_size, 3, "vertex_position");
    suzanne_depth_model->add_indices(suzanne_indices, suzanne_indices_size);

    glm::vec3 sun_position = glm::vec3(2.0, 2.0, 2.0);

    glm::mat4 perspective_transform = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
    glm::mat4 view_transform = glm::lookAt(sun_position, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 suzanne_model_transform = glm::translate(glm::mat4(), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 light_vp = perspective_transform * view_transform;
    suzanne_depth_model->add_uniform("mvp", &(light_vp * suzanne_model_transform)[0][0], 16);


    Model * floor_depth_model = Model::create_model(depth_shader);
    floor_depth_model->add_attribute(floor_vertices, sizeof(floor_vertices), 3, "vertex_position");
    floor_depth_model->add_indices(floor_indices, sizeof(floor_indices));
    floor_depth_model->add_uniform("mvp", &light_vp[0][0], 16);

    GLuint frame_buffer_id;
    glGenFramebuffers(1, &frame_buffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

    GLuint depth_texture;
    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glGenerateMipmap(GL_TEXTURE_2D);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

    glDrawBuffer(GL_NONE); // No color buffer is drawn to.

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "frame buffer is broken\n";
    }

    glUseProgram(depth_shader);

    suzanne_depth_model->render_model();

    delete suzanne_depth_model;


    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    GLuint shadow_shader = load_shaders("shaders/shadow.vsh", "", "shaders/shadow.fsh");

    Model * suzanne_model = Model::create_model(shadow_shader);
    suzanne_model->add_attribute(suzanne_vertices, suzanne_vertices_size, 3, "vertex_position");
    suzanne_model->add_attribute(suzanne_normals, suzanne_normals_size, 3, "vertex_normal");
    suzanne_model->add_indices(suzanne_indices, suzanne_indices_size);

    glm::vec3 color = glm::vec3(1.0,0.0,1.0);
    suzanne_model->add_uniform("color", &color[0], 3);

    glm::vec3 light_direction = glm::normalize(-sun_position);
    suzanne_model->add_uniform("light_direction", &light_direction[0], 3);

    view_transform = glm::lookAt(glm::vec3(1,3,8), glm::vec3(0,0,0), glm::vec3(0,1,0));

    glm::mat4 bias_matrix(
        0.5, 0.0, 0.0, 0.0, 
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );

    glm::mat4 biased_light_vp = bias_matrix * light_vp;
    suzanne_model->add_uniform("view_transform", &view_transform[0][0], 16);
    suzanne_model->add_uniform("model_transform", &suzanne_model_transform[0][0], 16);
    suzanne_model->add_uniform("light_mvp", &biased_light_vp[0][0], 16);

    free(suzanne_vertices);
    free(suzanne_normals);
    free(suzanne_indices);
    delete suzanne_geometry;


    Model * floor_model = Model::create_model(shadow_shader);

    floor_model->add_attribute(floor_vertices, sizeof(floor_vertices), 3, "vertex_position");
    floor_model->add_attribute(floor_normals, sizeof(floor_normals), 3, "vertex_normal");
    floor_model->add_indices(floor_indices, sizeof(floor_indices));

    color = glm::vec3(0.0,1.0,0.0);
    floor_model->add_uniform("color", &color[0], 3);
    floor_model->add_uniform("light_direction", &light_direction[0], 3);
    floor_model->add_uniform("view_transform", &view_transform[0][0], 16);
    floor_model->add_uniform("model_transform", &(glm::mat4())[0][0], 16);
    floor_model->add_uniform("light_mvp", &biased_light_vp[0][0], 16);



    GLint uniform_id = glGetUniformLocation(shadow_shader, "shadow_map");
    if (uniform_id == -1) {
        std::cout << "could not find attribute shadow_map.\n";
    } else {
        model_texture_t texture = {
            // CHECK ME
            .texture_unit = GL_TEXTURE0,
            .texture_id = depth_texture,
            .texture_uniform_id = uniform_id,
            .texture_type = GL_TEXTURE_2D
        };
        suzanne_model->textures.push_back(texture);
        floor_model->textures.push_back(texture);
    }

    models.emplace(shadow_shader, suzanne_model);
    models.emplace(shadow_shader, floor_model);

}
