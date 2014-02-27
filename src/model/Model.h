
#ifndef _MODEL_H
#define _MODEL_H

#include <GL/gl.h>
#include <vector>
#include <string>
#include <unordered_map>

typedef struct _model_attribute_t {
    GLuint buffer_id;
    GLint attribute_id;
    int num_channels;
} model_attribute_t;

typedef struct _model_texture_t {
    GLenum texture_unit;
    GLuint texture_id;
    GLint texture_uniform_id;
    GLenum texture_type;
} model_texture_t;

typedef struct _model_matrix_t {
    GLfloat * matrix_data;
    GLint matrix_id;
} model_matrix_t;

class Model {
    public:
        static Model * create_model(GLint shader_id);
        Model(std::vector<model_attribute_t> attributes, std::vector<model_texture_t> textures, std::unordered_map<std::string, model_matrix_t> matrices, GLint shader_id);
        void add_attribute(float * buffer, int size, int channels, std::string attribute_name);
        void add_indices(int * buffer, int size);
        void add_2d_texture(void * texture, int width, int height, GLenum type, int channels, std::string texture_name);
        void add_1d_texture(void * texture, int width, GLenum type, int channels, std::string texture_name);
        void add_uniform_matrix(std::string matrix_name, GLfloat * matrix);
        void set_num_instances(int n);
        void set_draw_mode(GLenum mode);
        GLint g_shader_program();
        void render_model();
    private:
        std::vector<model_attribute_t> attributes;
        std::vector<model_texture_t> textures;
        std::unordered_map<std::string, model_matrix_t> matrices;
        GLuint indices_buffer_id;
        int num_indices;
        int num_instances;
        GLint shader_id;
        GLenum draw_mode;
};

#endif


