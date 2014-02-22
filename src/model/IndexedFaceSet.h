
#include <GL/gl.h>
#include <vector>

#ifndef _INDEXED_FACE_SET_H
#define _INDEXED_FACE_SET_H

typedef struct _face_t {
    int degree;
    int * corners;
} face_t;

class IndexedFaceSet {
    public:
        IndexedFaceSet(int n_vertices, int n_faces);
        ~IndexedFaceSet();

        void add_vertex(float * vertex);
        void add_face(int degree, int * corners);
        void add_tex_coord(float * coord);
        void add_vertex_normal(float * normal);

        float * g_vertex(int ind);
        face_t * g_face(int ind);
        float * g_tex_coord(int ind);

        int g_num_vertices();
        int g_num_faces();

        float * g_model_buffer();
        float * g_vertex_normals_buffer();
        float * g_tex_buffer();

        int g_model_buffer_size();
        int g_vertex_normals_buffer_size();
        int g_tex_buffer_size();

        void normalize_vec(float * v);
        float * g_face_normal(int face_ind);
        void calculate_vertex_normals();

        float * g_bounding_sphere();
        float * g_bounding_cube();

    private:
        float * vertices;
        float * tex_coords;
        float * vertex_normals;
        face_t * faces;

        int num_vertices;
        int num_faces;
        int cur_face;
        int cur_vertex;
        int cur_tex_coord;
        int cur_vertex_normal;
};

#endif
