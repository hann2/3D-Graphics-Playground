
#ifndef _SCENE_H
#define _SCENE_H

#include <GL/glew.h>
#include "Model.h"
#include <unordered_map>
#include <string>

class Scene {
    private:
        static GLuint load_shader(std::string file_name, GLenum shader_type);
    protected:
        std::unordered_multimap<GLuint, Model *> models;
    public:
        Scene();
        virtual void setup_scene() = 0;
        void render(int w, int h);
        static GLuint load_shaders(std::string v_file, std::string g_file, std::string f_file);
};


#endif
