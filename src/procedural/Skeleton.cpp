
#include "Skeleton.h"
#include "load_collada.h"
#include <iostream>

Skeleton::Bone::Bone(glm::mat4 s) {
    state = s;
    children = std::vector<int>();
}

Skeleton::Skeleton() {
    bones = std::vector<Skeleton::Bone>();
}

IndexedFaceSet * Skeleton::generate_mesh() {
    static IndexedFaceSet * cylinder;
    if (cylinder == NULL) {
        cylinder = load_collada("assets/models/cylinder.dae");
    }
    IndexedFaceSet * mesh = new IndexedFaceSet(cylinder->g_num_vertices() * bones.size(), cylinder->g_num_faces() * bones.size());
    generate_mesh_h(mesh, bones.at(0), cylinder);
    return mesh;
}

void Skeleton::generate_mesh_h(IndexedFaceSet * mesh, Bone bone, IndexedFaceSet * cylinder) {
    int cur_ind = mesh->g_num_vertices();
    for (int vert_ind = 0; vert_ind < cylinder->g_num_vertices(); vert_ind++) {
        float * vert = cylinder->g_vertex(vert_ind);
        glm::vec4 vert_h(vert[0], vert[1], vert[2], 1.0);
        vert_h = bone.state * vert_h;
        mesh->add_vertex(&vert_h[0]);
    }
    for (int face_ind = 0; face_ind < cylinder->g_num_faces(); face_ind++) {
        int * old_corners = cylinder->g_face(face_ind)->corners;
        int * corners = (int *) malloc(sizeof(int) * 3);
        for (int i = 0; i < 3; i++) {
            corners[i] = old_corners[i] + cur_ind;
        }
        mesh->add_face(3, corners);
    }
    for (int child_ind : bone.children) {
        generate_mesh_h(mesh, bones.at(child_ind), cylinder);
    }
}

int Skeleton::add_bone(int parent, glm::mat4 state) {
    int ind = bones.size();
    bones.push_back(Bone(state));
    if (parent > -1) {
        if (ind == 0) {
            std::cout << "never added root\n";
        } else if (parent >= (int) bones.size()) {
            std::cout << "parent out of bounds\n";
        }
        bones.at(parent).children.push_back(ind);
    }
    return ind;
}



