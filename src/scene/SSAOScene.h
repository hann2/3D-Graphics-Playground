
#ifndef _SSAO_SCENE
#define _SSAO_SCENE

#include "DeferredScene.h"

class SSAOScene : public DeferredScene {
    public:
        void setup_scene();
    protected:
        void setup_g_buffers();
};

#endif

