

#include "TreeGenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

TreeGenerator::TreeGenerator(Turtle t, Skeleton s, branch_params_t branch_1, branch_params_t branch_2, float s_m) {
    turtle = t;
    skeleton = s;
    b1 = branch_1;
    b2 = branch_2;
    s_min = s_m;
    bone_stack = std::vector<int>();
}


Geometry * TreeGenerator::generate_tree() {
    float q = 0.6;
    float e = 0.5;
    branch_params_t b1 = {
        .alpha = 30.0,
        .phi = 70.0,
        .s_scale = 0.9,
        .w_scale = (float) pow(q, e)
    };
    branch_params_t b2 = {
        .alpha = -50.0,
        .phi = 100.0,
        .s_scale = 0.8,
        .w_scale = (float) pow(1 - q, e)
    };
    TreeGenerator t(Turtle(), Skeleton(), b1, b2, 0.20);
    t.generate_tree_h(1.0, 0.2);
    return t.skeleton.generate_mesh();
}

void TreeGenerator::generate_tree_h(float s, float w) {
    if (s < s_min) {
        return;
    }

    int parent;
    if (bone_stack.empty()) {
        parent = -1;
    } else {
        parent = bone_stack.back();
    }

    // scale for length and radius of cylinder
    glm::mat4 bone_state = *turtle.peek_state() * glm::translate(glm::mat4(), glm::vec3(0.0f,0.0f,0.5f * s)) * glm::scale(glm::mat4(), glm::vec3(w,w,s));
    int bone_ind = skeleton.add_bone(parent, bone_state);
    turtle.forward(s);

    bone_stack.push_back(bone_ind);
    // branch 1
    turtle.push_state();
    turtle.yaw(b1.alpha);
    turtle.roll(b1.phi);
    generate_tree_h(s * b1.s_scale, w * b1.w_scale);
    turtle.pop_state();

    // branch 2
    turtle.push_state();
    turtle.yaw(b2.alpha);
    turtle.roll(b2.phi);
    generate_tree_h(s * b2.s_scale, w * b2.w_scale);
    turtle.pop_state();
    bone_stack.pop_back();
}

