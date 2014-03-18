
#include <string.h>
#include <iostream>
#include <GL/glew.h>
#include "Model.h"

Model * Model::create_model(GLint shader_id) {
    return new Model(shader_id);
}

Model::Model(GLint s) {
    attributes = std::vector<model_attribute_t>();
    textures = std::vector<model_texture_t>();
    uniforms = std::unordered_map<std::string, model_uniform_t>();
    num_indices = -1;
    indices_buffer_id = -1;
    num_instances = -1;
    shader_id = s;
    draw_mode = GL_TRIANGLES;
}

void Model::set_num_instances(int n) {
    num_instances = n;
}

void Model::set_draw_mode(GLenum mode) {
    draw_mode = mode;
}

GLint Model::g_shader_program() {
    return shader_id;
}

// always set vertices first
void Model::add_attribute(float * buffer, int size, int channels, std::string attribute_name) {
    GLint attribute_location = glGetAttribLocation(shader_id, attribute_name.c_str());
    if (attribute_location == -1) {
        std::cout << "could not find attribute " << attribute_name << ".\n";
        return;
    }

    GLuint buffer_id;
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);

    model_attribute_t attribute = {
        .buffer_id = buffer_id,
        .attribute_id = attribute_location,
        .num_channels = channels
    };

    attributes.push_back(attribute);
}

void Model::add_indices(int * buffer, int size) {
    glGenBuffers(1 , &indices_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
    num_indices = size;
}

void Model::add_2d_texture(void * data, int width, int height, GLenum type, int channels, std::string texture_name) {
    GLint uniform_id = glGetUniformLocation(shader_id, texture_name.c_str());
    if (uniform_id == -1) {
        std::cout << "could not find attribute " << texture_name << ".\n";
        return;
    }

    static const GLenum formats [] = {
        GL_RED, GL_RG, GL_RGB, GL_RGBA
    };
    GLenum format = formats[channels - 1];

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    GLuint texture_unit = GL_TEXTURE0 + textures.size();

    model_texture_t texture = {
        .texture_unit = texture_unit,
        .texture_id = texture_id,
        .texture_uniform_id = uniform_id,
        .texture_type = GL_TEXTURE_2D
    };

    textures.push_back(texture);
}

void Model::add_1d_texture(void * data, int width, GLenum type, int channels, std::string texture_name) {
    GLint uniform_id = glGetUniformLocation(shader_id, texture_name.c_str());
    if (uniform_id == -1) {
        std::cout << "could not find attribute " << texture_name << ".\n";
        return;
    }

    static const GLenum formats [] = {
        GL_RED, GL_RG, GL_RGB, GL_RGBA
    };
    GLenum format = formats[channels - 1];

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_1D, texture_id);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, format, width, 0, format, type, data);
    GLuint texture_unit = GL_TEXTURE0 + textures.size();

    model_texture_t texture = {
        .texture_unit = texture_unit,
        .texture_id = texture_id,
        .texture_uniform_id = uniform_id,
        .texture_type = GL_TEXTURE_1D
    };

    textures.push_back(texture);
}

void Model::add_uniform(std::string uniform_name, float * data, int channels) {
    add_uniform(uniform_name, data, channels, 1);
}

void Model::add_uniform(std::string uniform_name, float * data, int channels, int n) {
    add_uniform_h(uniform_name, data, channels, n, GL_FLOAT, sizeof(float));
}

void Model::add_uniform(std::string uniform_name, int * data, int channels) {
    add_uniform(uniform_name, data, channels, 1);
}

void Model::add_uniform(std::string uniform_name, int * data, int channels, int n) {
    add_uniform_h(uniform_name, data, channels, n, GL_INT, sizeof(int));
}

void Model::add_uniform_h(std::string uniform_name, void * data, int channels, int n, GLenum type, size_t size) {
    GLint uniform_location = glGetUniformLocation(shader_id, uniform_name.c_str());
    if (uniform_location == -1) {
        std::cout << "could not find attribute " << uniform_name << ".\n";
        return;
    }

    if (uniforms.count(uniform_name) == 0) {
        GLfloat * copied_data = (GLfloat *) malloc(size * channels * n);
        memcpy(copied_data, data, size * channels * n);
        model_uniform_t uniform = {
            .data = copied_data,
            .location = uniform_location,
            .type = type,
            .channels = channels,
            .n = n
        };
        uniforms.emplace(uniform_name, uniform);
    } else {
        model_uniform_t uniform = uniforms.at(uniform_name);
        memcpy(uniform.data, data, size * channels * n);
    }
}

void Model::render_model() {
    for (auto kvPair : uniforms) {
        model_uniform_t u = kvPair.second;
        if (u.type == GL_FLOAT) {
            switch (u.channels) {
                case 1:
                    glUniform1fv(u.location, u.n, (GLfloat *) u.data);
                    break;
                case 2:
                    glUniform2fv(u.location, u.n, (GLfloat *) u.data);
                    break;
                case 3:
                    glUniform3fv(u.location, u.n, (GLfloat *) u.data);
                    break;
                case 4:
                    glUniform4fv(u.location, u.n, (GLfloat *) u.data);
                    break;
                case 9:
                    glUniformMatrix3fv(u.location, u.n, GL_FALSE, (GLfloat *) u.data);
                    break;
                case 16:
                    glUniformMatrix4fv(u.location, u.n, GL_FALSE, (GLfloat *) u.data);
                    break;
            }
        } else if (u.type == GL_INT){
            switch (u.channels) {
                case 1:
                    glUniform1i(u.location, *((GLint *) u.data));
                    break;
            }

        }
    }

    for (model_texture_t texture : textures) {
        glActiveTexture(texture.texture_unit);
        glBindTexture(texture.texture_type, texture.texture_id);
        glUniform1i(texture.texture_uniform_id, 0);
    }

    for (model_attribute_t attribute : attributes) {
        glEnableVertexAttribArray(attribute.attribute_id);
        glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer_id);
        glVertexAttribPointer(attribute.attribute_id, attribute.num_channels, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_id);
    if (num_instances > 0) {
        glDrawElementsInstanced(draw_mode, num_indices, GL_UNSIGNED_INT, NULL, num_instances);
    } else {
        glDrawElements(draw_mode, num_indices, GL_UNSIGNED_INT, NULL);
    }

    for (model_attribute_t attribute : attributes) {
        glDisableVertexAttribArray(attribute.attribute_id);
    }
}

