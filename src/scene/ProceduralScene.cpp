

#include "ProceduralScene.h"
#include "ProceduralGenerator.h"
#include "TreeGenerator.h"
#include "Model.h"
#include "IndexedFaceSet.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void ProceduralScene::setup_scene() {
    GLint grass_shader = load_shaders("shaders/grass.vsh", "", "shaders/grass.fsh");
    Model * terrain_model = Model::create_model(grass_shader);

    int mesh_size = 64;
    float * terrain = terrain_height(mesh_size, mesh_size);
    float tex_step = 1.0 / (mesh_size - 1);

    IndexedFaceSet geometry(mesh_size * mesh_size, (mesh_size - 1) * (mesh_size - 1) * 2);

    for (int i = 0; i < mesh_size; i++) {
        for (int j = 0; j < mesh_size; j++) {
            float vertex [] = {
                (float)i,
                terrain[i * mesh_size + j] * 20,
                (float)j
            };
            float texture_coord [] = {
                i * tex_step,
                j * tex_step
            };
            geometry.add_vertex(&vertex[0]);
            geometry.add_texture_coordinate(&texture_coord[0]);
        }
    }

    for (int i = 0; i < mesh_size - 1; i++) {
        for (int j = 0; j < mesh_size - 1; j++) {
            int face1 [] = {
                i * mesh_size + j,
                i * mesh_size + j + 1,
                (i + 1) * mesh_size + j + 1
            };
            int face2 [] = {
                (i + 1) * mesh_size + j + 1,
                (i + 1) * mesh_size + j,
                i * mesh_size + j
            };
            geometry.add_face(3, &face1[0]);
            geometry.add_face(3, &face2[0]);
        }
    }

    float * terrain_vertices = geometry.g_vertex_buffer();
    int * terrain_indices = geometry.g_index_buffer();
    float * terrain_normals = geometry.g_vertex_normal_buffer();
    float * terrain_tex_coords = geometry.g_texture_coordinate_buffer();

    int terrain_vertices_size = geometry.g_vertex_buffer_size();
    int terrain_indices_size = geometry.g_index_buffer_size();
    int terrain_normals_size = geometry.g_vertex_normal_buffer_size();
    int terrain_tex_coords_size = geometry.g_texture_coordinate_buffer_size();

    terrain_model->add_attribute(terrain_vertices, terrain_vertices_size, 3, "vertex_position");
    terrain_model->add_attribute(terrain_normals, terrain_normals_size, 3, "vertex_normal");
    terrain_model->add_attribute(terrain_tex_coords, terrain_tex_coords_size, 2, "vertex_UV");
    terrain_model->add_indices(terrain_indices, terrain_indices_size);

    float * grass_text = grass_texture(256, 256);
    terrain_model->add_texture(grass_text, 256, 256, GL_FLOAT, 1, "texture_sampler");

    glm::mat4 view_transform = glm::lookAt(glm::vec3(50,20,10), glm::vec3(32, 10, 32), glm::vec3(0,1,0));
    glm::mat4 model_transform = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
    terrain_model->add_uniform_matrix("view_transform", &view_transform[0][0]);
    terrain_model->add_uniform_matrix("model_transform", &model_transform[0][0]);
    models.emplace(grass_shader, terrain_model);

    free(terrain_vertices);
    free(terrain_indices);
    free(terrain_normals);
    free(terrain_tex_coords);
    free(grass_text);

    GLint wire_shader = load_shaders("shaders/default.vsh", "shaders/wire.gsh", "shaders/wire.fsh");

    Model * tree_model = Model::create_model(wire_shader);
    IndexedFaceSet * tree_geometry = TreeGenerator::generate_normal_tree();

    float * tree_vertices = tree_geometry->g_vertex_buffer();
    int * tree_indices = tree_geometry->g_index_buffer();
    int tree_vertices_size = tree_geometry->g_vertex_buffer_size();
    int tree_indices_size = tree_geometry->g_index_buffer_size();
    tree_model->add_attribute(tree_vertices, tree_vertices_size, 3, "vertex_position");
    tree_model->add_indices(tree_indices, tree_indices_size);

    model_transform =
        glm::translate(glm::mat4(), glm::vec3(32.0, terrain[32 * mesh_size + 32] * 20, 32.0)) *
        glm::rotate(glm::mat4(), -90.0f, glm::vec3(1.0, 0.0, 0.0));
    tree_model->add_uniform_matrix("view_transform", &view_transform[0][0]);
    tree_model->add_uniform_matrix("model_transform", &model_transform[0][0]);
    models.emplace(wire_shader, tree_model);

    free(tree_vertices);
    free(tree_indices);
    free(terrain);
    delete tree_geometry;
}

