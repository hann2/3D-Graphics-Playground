

#include <GL/glew.h>
#include "TreeGenerator.h"
#include "load_cylinder.h"
#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Model * TreeGenerator::generate_normal_tree() {
    branch_params_t b1 = {
        .alpha = 30.0,
        .phi = 137.0,
        .s_scale = 0.8,
        .w_scale = 0.7071
    };
    branch_params_t b2 = {
        .alpha = -30.0,
        .phi = 137.0,
        .s_scale = 0.8,
        .w_scale = 0.7071
    };
    return generate_tree(b1, b2, 0.2, 1.0, 0.2);
}

Model * TreeGenerator::generate_willow_tree() {
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
    return generate_tree(b1, b2, 0.2, 1.0, 0.2);
}

Model * TreeGenerator::generate_tree(branch_params_t b1, branch_params_t b2, float s_min, float s, float w) {
    IndexedFaceSet * cylinder = load_cylinder(6);
    IndexedFaceSet * geometry = new IndexedFaceSet();
    Turtle * turtle = new Turtle();

    TreeGenerator t(geometry, cylinder, turtle, b1, b2, s_min);
    t.generate_tree_h(s, w);

    Model * tree_model = Model::create_model(Scene::load_shaders("shaders/tree.vsh", "shaders/wire.gsh", "shaders/wire.fsh"));

    float * tree_vertices = geometry->g_vertex_buffer();
    // float * tree_normals = geometry->g_vertex_normal_buffer();
    // float * tree_tex_coords = geometry->g_texture_coordinate_buffer();

    int tree_vertices_size = geometry->g_vertex_buffer_size();
    // int tree_normals_size = geometry->g_vertex_normal_buffer_size();
    // int tree_tex_coords_size = geometry->g_texture_coordinate_buffer_size();

    tree_model->add_attribute(tree_vertices, tree_vertices_size, 3, "vertex_position");
    // tree_model->add_attribute(tree_normals, tree_normals_size, 3, "vertex_normal");
    // tree_model->add_attribute(tree_tex_coords, tree_tex_coords_size, 2, "vertex_UV");

    free(tree_vertices);
    // free(tree_normals);
    // free(tree_tex_coords);

    // float * grass_text = grass_texture(256, 256);
    // m->add_2d_texture(grass_text, 256, 256, GL_FLOAT, 1, "texture_sampler");

    tree_model->add_indices(geometry->g_index_buffer(), geometry->g_index_buffer_size());

    // triangle strips are slower so dont use this
    // tree_model->add_indices(&t.indices[0], t.indices.size() * sizeof(int));
    // tree_model->set_draw_mode(GL_TRIANGLE_STRIP);

    return tree_model;
}

TreeGenerator::TreeGenerator(IndexedFaceSet * m, IndexedFaceSet * c, Turtle * t, branch_params_t branch_1, branch_params_t branch_2, float s_m) {
    mesh = m;
    cylinder = c;
    turtle = t;
    b1 = branch_1;
    b2 = branch_2;
    s_min = s_m;
    indices = std::vector<int>();
}

void TreeGenerator::generate_tree_h(float s, float w) {
    if (s < s_min) {
        return;
    }

    add_bone(s, w);
    turtle->forward(s);

    // branch 1
    turtle->push_state();
    turtle->yaw(b1.alpha);
    turtle->roll(b1.phi);
    generate_tree_h(s * b1.s_scale, w * b1.w_scale);
    turtle->pop_state();

    // branch 2
    turtle->push_state();
    turtle->yaw(b2.alpha);
    turtle->roll(b2.phi);
    generate_tree_h(s * b2.s_scale, w * b2.w_scale);
    turtle->pop_state();
}

void TreeGenerator::add_bone(float s, float w) {
    glm::mat4 bone = 
        // tree grows into z-axis, we want it growing up
        glm::rotate(glm::mat4(), -90.0f, glm::vec3(1.0, 0.0, 0.0)) *
        *turtle->peek_state() *
        // scale for length and radius of cylinder
        glm::scale(glm::mat4(), glm::vec3(w,w,s));

    int cur_ind = mesh->g_num_vertices();
    indices.push_back(cur_ind);

    for (int vert_ind = 0; vert_ind < cylinder->g_num_vertices(); vert_ind++) {
        float * vert = cylinder->g_vertex(vert_ind);
        glm::vec4 vert_h = bone * glm::vec4(vert[0], vert[1], vert[2], 1.0);
        mesh->add_vertex(&vert_h[0]);

        // float * normal = cylinder->g_vertex_normal(vert_ind);
        // glm::vec4 normal_h = bone * glm::vec4(normal[0], normal[1], normal[2], 0.0);
        // mesh->add_vertex_normal(&normal_h[0]);

        // float * tex_coord = cylinder->g_texture_coordinate(vert_ind);
        // mesh->add_texture_coordinate(tex_coord);
    }

    for (int face_ind = 0; face_ind < cylinder->g_num_faces(); face_ind++) {
        int * old_corners = cylinder->g_face(face_ind)->corners;
        int * corners = (int *) malloc(sizeof(int) * 3);
        for (int i = 0; i < 3; i++) {
            corners[i] = old_corners[i] + cur_ind;
        }
        indices.push_back(cur_ind + face_ind);
        mesh->add_face(3, corners);
    }

    indices.push_back(indices.back());
}
