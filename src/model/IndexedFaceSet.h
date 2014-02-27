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
        IndexedFaceSet();
        ~IndexedFaceSet();

        void add_vertex(float * v);
        void add_face(int degree, int * corners);
        void add_texture_coordinate(float * coord);
        void add_vertex_normal(float * normal);

        float * g_vertex(int ind);
        face_t * g_face(int ind);
        float * g_texture_coordinate(int ind);
        float * g_vertex_normal(int ind);

        int g_num_vertices();
        int g_num_faces();

        float * g_vertex_buffer();
        int * g_index_buffer();
        float * g_vertex_normal_buffer();
        float * g_texture_coordinate_buffer();

        int g_vertex_buffer_size();
        int g_index_buffer_size();
        int g_vertex_normal_buffer_size();
        int g_texture_coordinate_buffer_size();
        
        void calculate_vertex_normals();

        float * g_bounding_sphere();
        float * g_bounding_cube();
    private:

        void normalize_vec(float * v);
        float * g_face_normal(int face_ind);

        std::vector<float> vertices;
        std::vector<float> tex_coords;
        std::vector<float> vertex_normals;
        std::vector<face_t> faces;

        int num_triangles;
};

#endif
