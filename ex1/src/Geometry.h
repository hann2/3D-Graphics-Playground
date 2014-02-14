
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

        float * const g_vertex(int ind);
        face_t * const g_face(int ind);
        int const g_num_vertices();
        int const g_num_faces();
        float * const g_model_buffer();
        float * g_centroid();
        float g_bounding_radius();
        float * g_bounding_cube();

        static GLfloat * transformation_matrix(geometric_state_t * state);
        static GLfloat * interp_matrix(std::vector<geometric_state_t *> * states, std::vector<float> * weights);

    private:
        float * vertices;
        face_t * faces;
        bone_t * bones;
        vertex_weights_t * weights;

        int num_vertices;
        int num_faces;
        int cur_face;
        int cur_vertex;
        float radius;
        float * centroid;
        float * cube;

        Geometry& operator=(Geometry const&);
        void const calc_centroid();
        void const calc_bounding_radius();
        void const calc_bounding_cube();
};

#endif
