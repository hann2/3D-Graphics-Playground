
#ifndef _WATER_SCENE_H
#define _WATER_SCENE_H

#include "Scene.h"
#include "Model.h"

class WaterScene : public Scene {
    public:
        void setup_scene();
        void render(int w, int h, float t);
    private:
        GLint parallax_shader;
        Model * wall;
        GLint water_shader;
        Model * water;
        GLint particle_shader;
        Model * particles;
};

#endif
