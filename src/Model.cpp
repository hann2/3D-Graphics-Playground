
#include <string.h>
#include <iostream>
#include <GL/glew.h>
#include "Model.h"

Model * Model::create_model(GLuint program_id) {
    return new Model(std::vector<model_attribute_t>(), std::vector<model_texture_t>(), std::unordered_map<std::string, model_matrix_t>(), program_id);
}

Model::Model(std::vector<model_attribute_t> attributes,
            std::vector<model_texture_t> textures,
            std::unordered_map<std::string, model_matrix_t> matrices,
            GLuint program_id) {
    this->attributes = attributes;
    this->textures = textures;
    this->matrices = matrices;
    this->shader_program_id = program_id;
    this->model_size = -1;
}

// always set vertices first
void Model::add_attribute(float * buffer, int size, int channels, std::string attribute_name) {
    GLint attribute_location = glGetAttribLocation(shader_program_id, attribute_name.c_str());
    if (attribute_location == -1) {
        std::cout << "could not find attribute " << attribute_name << ".\n";
        return;
    }

    if (model_size == -1) {
        model_size = size;
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

void Model::add_texture(void * data, int width, int height, GLenum type, int channels, std::string texture_name) {
    GLint uniform_id = glGetUniformLocation(shader_program_id, texture_name.c_str());
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
        .texture_uniform_id = uniform_id
    };

    textures.push_back(texture);
}

// must be 4x4
void Model::add_uniform_matrix(std::string matrix_name, GLfloat * data) {
    GLint matrix_location = glGetUniformLocation(shader_program_id, matrix_name.c_str());
    if (matrix_location == -1) {
        std::cout << "could not find attribute " << matrix_name << ".\n";
        return;
    }

    if (matrices.count(matrix_name) == 0) {
        model_matrix_t matrix = {
            .matrix_data = data,
            .matrix_id = matrix_location
        };
        matrices.emplace(matrix_name, matrix);
    } else {
        model_matrix_t m = matrices.at(matrix_name);
        free(m.matrix_data);
        m.matrix_data = data;
    }
}

void Model::render_model() {
    glUseProgram(shader_program_id);

    for (auto kvPair : matrices) {
        model_matrix_t matrix = kvPair.second;
        glUniformMatrix4fv(matrix.matrix_id, 1, GL_FALSE, matrix.matrix_data);
    }

    for (auto texture : textures) {
        glActiveTexture(texture.texture_unit);
        glBindTexture(GL_TEXTURE_2D, texture.texture_id);
        glUniform1i(texture.texture_uniform_id, 0);
    }

    for (auto attribute : attributes) {
        glEnableVertexAttribArray(attribute.attribute_id);
        glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer_id);
        glVertexAttribPointer(attribute.attribute_id, attribute.num_channels, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    }

    glDrawArrays(GL_TRIANGLES, 0, model_size);

    for (auto attribute : attributes) {
        glDisableVertexAttribArray(attribute.attribute_id);
    }

    glUseProgram(0);
}

