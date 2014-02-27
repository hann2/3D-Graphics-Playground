

#ifndef _TREE_GENERATOR_H
#define _TREE_GENERATOR_H

#include "Model.h"
#include "IndexedFaceSet.h"
#include "Turtle.h"
#include <vector>

typedef struct _branch_params_t {
    float alpha;
    float phi;
    float s_scale;
    float w_scale;
} branch_params_t;

class TreeGenerator {
    public:
        static Model * generate_normal_tree();
        static Model * generate_willow_tree();
        static Model * generate_tree(branch_params_t b1, branch_params_t b2, float s_min, float s, float w);
        TreeGenerator(IndexedFaceSet * m, IndexedFaceSet * c, Turtle * t, branch_params_t branch_1, branch_params_t branch_2, float s_m);
    private:
        void generate_tree_h(float s, float w);
        void add_bone(float s, float w);
        IndexedFaceSet * mesh;
        IndexedFaceSet * cylinder;
        Turtle * turtle;
        branch_params_t b1;
        branch_params_t b2;
        float s_min;
        std::vector<int> indices;
};

#endif
