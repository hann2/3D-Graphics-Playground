

#ifndef _DEFERRED_SCENE
#define _DEFERRED_SCENE

#include <vector>
#include "Scene.h"
#include "Model.h"

typedef struct _g_buffer_t {
    int ind;
    GLuint id;
    GLint location;
} g_buffer_t;


class DeferredScene : public Scene {
    public:
        DeferredScene();
        virtual void setup_scene() = 0;
        void render(int w, int h);
    protected:
        void add_g_buffer(GLenum internal, GLenum format, GLenum type, std::string attribute_name);
        virtual void setup_g_buffers() = 0;
        Model * billboard_model;
        GLint geometry_pass_shader;
        GLint lighting_pass_shader;
    private:
        void resize();
        void setup_deferred();
        std::vector<g_buffer_t> g_buffers;
        int window_width;
        int window_height;
        GLuint fboName;
        GLuint fRenderBuffer;
        GLenum * texture_units;
};

#endif


