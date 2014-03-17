
#include "DeferredScene.h"
#include <iostream>

DeferredScene::DeferredScene() {
    window_width = -1;
    window_height = -1;
    // g_buffers = std::vector<g_buffer_t>();
    billboard_model = NULL;
}

void DeferredScene::setup_deferred() {
    glGenFramebuffers(1, &fboName);
    glGenRenderbuffers(1, &fRenderBuffer);

    // Bind the depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, fRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, window_width, window_height);

    // Bind the FBO so that the next operations will be bound to it.
    glBindFramebuffer(GL_FRAMEBUFFER, fboName);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fRenderBuffer);

    setup_g_buffers();

    texture_units = (GLenum *) malloc(sizeof(GLenum) * g_buffers.size());
    for (unsigned int i = 0; i < g_buffers.size(); i++) {
        texture_units[i] = GL_COLOR_ATTACHMENT0 + i;
    }
}

void DeferredScene::add_g_buffer(GLenum internal, GLenum format, GLenum type, std::string attribute_name) {

    GLint light_pass_uniform_id = glGetUniformLocation(lighting_pass_shader, attribute_name.c_str());
    if (light_pass_uniform_id == -1) {
        std::cout << "could not find attribute " << attribute_name << " in lighting pass.\n";
        return;
    }

    GLuint g_buffer_id;
    glGenTextures(1, &g_buffer_id);
    glBindTexture(GL_TEXTURE_2D, g_buffer_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, internal, window_width, window_height, 0, format, type, 0);

    int ind = g_buffers.size();
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + ind, GL_TEXTURE_2D, g_buffer_id, 0);

    g_buffer_t g_buffer = {
        .ind = ind,
        .id = g_buffer_id,
        .location = light_pass_uniform_id
    };

    g_buffers.push_back(g_buffer);
}

void DeferredScene::resize() {
    for (g_buffer_t g_buffer : g_buffers) {
        glDeleteTextures(1, &g_buffer.id);
    }
    g_buffers.clear();
    glDeleteRenderbuffers(1, &fRenderBuffer);
    glDeleteFramebuffers(1, &fboName);
    setup_deferred();
}

void DeferredScene::render(int w, int h) {
    if (w != window_width || h != window_height) {
        window_width = w;
        window_height = h;
        resize();
    }

    // GEOMETRY PASS
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboName);
    glDrawBuffers(g_buffers.size(), texture_units);
    glClearColor(0.0f, 0.0f, -1000.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Execute the first render stage, which will fill out the internal buffers with data:
    Scene::render(w, h);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // LIGHTING PASS
    glUseProgram(lighting_pass_shader);

    glClearColor(0.0f, 0.3f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (g_buffer_t g_buffer : g_buffers) {
        glActiveTexture(GL_TEXTURE0 + g_buffer.ind);
        glBindTexture(GL_TEXTURE_2D, g_buffer.id);
        glUniform1i(g_buffer.location, g_buffer.ind);
    }

    billboard_model->render_model();
}

