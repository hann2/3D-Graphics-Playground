
#include "Scene.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene() {
    models = std::unordered_multimap<GLuint, Model *>();
}

void Scene::render(int w, int h) {
    glm::mat4 perspective_transform = glm::perspective(45.0f, ((float)w) / h, 0.1f, 1000.0f);

    GLint shader_program_id = -1;
    for (unsigned int i = 0; i < models.bucket_count(); i++) {
        for (auto it = models.begin(i); it != models.end(i); it++) {
            GLint temp = it->first;
            if (temp != shader_program_id) {
                shader_program_id = temp;
                glUseProgram(shader_program_id);
            }
            Model * model = it->second;
            model->add_uniform("perspective_transform", &perspective_transform[0][0], 16);
            model->render_model();
        }
    }
}

void Scene::render(int w, int h, float t) {
    glm::mat4 perspective_transform = glm::perspective(45.0f, ((float)w) / h, 0.1f, 1000.0f);

    GLint shader_program_id = -1;
    for (unsigned int i = 0; i < models.bucket_count(); i++) {
        for (auto it = models.begin(i); it != models.end(i); it++) {
            GLint temp = it->first;
            if (temp != shader_program_id) {
                shader_program_id = temp;
                glUseProgram(shader_program_id);
            }
            Model * model = it->second;
            model->add_uniform("perspective_transform", &perspective_transform[0][0], 16);
            model->add_uniform("t", &t, 1);

            model->render_model();
        }
    }
}

GLuint Scene::load_shader(std::string file_name, GLenum shader_type) {
    GLuint id = glCreateShader(shader_type);
    std::string shader_code;
    std::ifstream file_stream(file_name);
    if (file_stream.fail()) {
        std::cout << "could not find shader file " << file_name << "\n";
        throw 0;
    }

    file_stream.seekg(0, std::ios::end);   
    shader_code.reserve(file_stream.tellg());
    file_stream.seekg(0, std::ios::beg);

    shader_code.assign((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());

    char const * source_pointer = shader_code.c_str();
    glShaderSource(id, 1, &source_pointer , NULL);
    glCompileShader(id);

    int log_length;
    GLint result = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_length);
    if ( log_length > 1 ){
        char * error_message = (char *) calloc(log_length + 1, sizeof(char));
        glGetShaderInfoLog(id, log_length, NULL, error_message);
        std::cout << "Error when compiling shader " << file_name << "\n";
        std::cout << error_message << "\n";
        free(error_message);
    }

    return id;
}

GLuint Scene::load_shaders(std::string v_file, std::string g_file, std::string f_file) {
    GLuint program_id = glCreateProgram();

    GLuint v_id = load_shader(v_file, GL_VERTEX_SHADER);
    glAttachShader(program_id, v_id);
    GLuint g_id;
    if (g_file != "") {
        g_id = load_shader(g_file, GL_GEOMETRY_SHADER);
        glAttachShader(program_id, g_id);
    }
    GLuint f_id = load_shader(f_file, GL_FRAGMENT_SHADER);
    glAttachShader(program_id, f_id);

    glLinkProgram(program_id);

    int log_length;
    GLint result = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
    if ( log_length > 1 ){
        char * error_message = (char *) calloc(log_length + 1, sizeof(char));
        glGetProgramInfoLog(program_id, log_length, NULL, error_message);
        printf("%s\n", error_message);
        free(error_message);
    }

    glDeleteShader(v_id);
    if (g_file != "") {
        glDeleteShader(g_id);
    }
    glDeleteShader(f_id);

    return program_id;
}

