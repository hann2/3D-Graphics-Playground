
#include <GL/gl.h>
#include <vector>

#ifndef _GEOMETRY_H
#define _GEOMETRY_H

typedef enum {
    COLLADA
} file_type_t;

typedef struct _face_t {
    int degree;
    int * corners;
} face_t;

typedef struct _geometric_state_t {
    float orientation [4];
    float position [3];
    float scale [3];
} geometric_state_t;

typedef struct _bone_t {
    struct _bone_t * parent;
    geometric_state_t * default_state;
    GLfloat * default_transform;
} bone_t;

typedef struct _vertex_weights_t {
    int num_bones;
    bone_t * bones;
    float * weights;
} vertex_weights_t;

class Geometry {
    public:
        Geometry(int num_vertices, int num_faces);
        ~Geometry();

        void add_vertex(float * vertex);
        void add_face(int degree, int * corners);
        void add_tex_coord(int ind, float * coords);

        float * g_vertex(int ind);
        face_t * g_face(int ind);
        float * g_tex_coord(int ind);
        int g_num_vertices();
        int g_num_faces();
        int g_model_buffer_size();
        int g_normals_buffer_size();
        int g_tex_buffer_size();
        float * g_model_buffer();
        float * g_normals_buffer();
        float * g_tex_buffer();
        float * g_bounding_sphere();
        float * g_bounding_cube();
        void normalize_vec(float * v);
        float * g_face_normal(int face_ind);

    private:
        float * vertices;
        float * tex_coords;
        face_t * faces;
        bone_t * bones;
        vertex_weights_t * weights;

        int num_vertices;
        int num_faces;
        int cur_face;
        int cur_vertex;
};

#endif
