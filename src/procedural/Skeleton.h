
#ifndef _SKELETON_H
#define _SKELETON_H

#include "IndexedFaceSet.h"
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
        void generate_mesh_h(IndexedFaceSet * mesh, Bone bone, IndexedFaceSet * cylinder);
    public:
        Skeleton();
        IndexedFaceSet * generate_mesh();
        int add_bone(int parent, glm::mat4 state);
};

#endif
