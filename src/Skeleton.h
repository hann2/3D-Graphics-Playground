
#ifndef _SKELETON_H
#define _SKELETON_H

#include "Geometry.h"
#include <glm/glm.hpp>
#include <vector>

class Skeleton {
    private:
        class Bone {
            public:
                Bone(glm::mat4 state);
                glm::mat4 state;
                std::vector<int> children;
        };
        std::vector<Bone> bones;
        void generate_mesh_h(Geometry * mesh, Bone bone, Geometry * cylinder);
    public:
        Skeleton();
        Geometry * generate_mesh();
        int add_bone(int parent, glm::mat4 state);
};

#endif
