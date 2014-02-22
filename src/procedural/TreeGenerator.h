

#ifndef _TREE_GENERATOR_H
#define _TREE_GENERATOR_H

#include "IndexedFaceSet.h"
#include "Turtle.h"
#include "Skeleton.h"
#include <vector>

typedef struct _branch_params_t {
    float alpha;
    float phi;
    float s_scale;
    float w_scale;
} branch_params_t;

class TreeGenerator {
    public:
        static IndexedFaceSet * generate_tree();
        TreeGenerator(Turtle t, Skeleton s, branch_params_t branch_1, branch_params_t branch_2, float s_m);
    private:
        void generate_tree_h(float s, float w);
        std::vector<int> bone_stack;
        Turtle turtle;
        Skeleton skeleton;
        branch_params_t b1;
        branch_params_t b2;
        float s_min;
};

#endif

