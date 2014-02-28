

#ifndef _OUTLINE_SCENE
#define _OUTLINE_SCENE

#include <vector>
#include "Scene.h"
#include "Model.h"

typedef struct _g_buffer_t {
    int ind;
    GLuint g_buffer_id;
    GLint light_pass_uniform_id;
} g_buffer_t;


class OutlineScene : Scene {
    public:
        OutlineScene(int w, int h);
        void setup_scene();
        void add_g_buffer(int channels, GLenum type, std::string attribute_name);
        void render(int w, int h);
    private:
        std::vector<g_buffer_t> g_buffers;
        std::vector<Model *> models;
        Model * billboard;
        GLint geometry_pass_shader;
        GLint lighting_pass_shader;
        int window_width;
        int window_height;
        Model * billboard_model;
        GLuint fbo;
};

#endif
