
#ifndef _TOON_SCENE
#define _TOON_SCENE

#include "DeferredScene.h"

class ToonScene : public DeferredScene {
    public:
        void setup_scene();
    protected:
        void setup_g_buffers();
};

#endif

