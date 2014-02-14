

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdexcept>
#include <math.h>
#include "Geometry.h"

#define DIMENSION 3
#define VERTEX_SIZE (sizeof(float) * DIMENSION)

Geometry::Geometry(int num_vertices, int num_faces) {
    cur_face = 0;
    cur_vertex = 0;
    this->num_vertices = num_vertices;
    this->num_faces = num_faces;
    radius = -1.0;
    centroid = NULL;
    cube = NULL;
    vertices = (float *) malloc(num_vertices * VERTEX_SIZE);
    faces = (face_t *) calloc(num_faces, sizeof(face_t));
    bones = NULL;
    weights = NULL;
}

Geometry::~Geometry() {
    free(vertices);
    for (int ind = 0; ind < cur_face; ind++) {
        free(faces[ind].corners);
    }
    free(faces);
}

void Geometry::add_vertex(float * vertex) {
    if (cur_vertex == num_vertices) {
        printf("Geometry is full, cannot add vertex.\n");
        throw std::out_of_range("out of range");
    }

    //copy vertex directly into vertices
    memcpy(vertices + (cur_vertex * DIMENSION), vertex, VERTEX_SIZE);

    cur_vertex++;
}

void Geometry::add_face(int degree, int * corners) {
    if (cur_face == num_faces) {
        printf("Geometry is full, cannot add face.\n");
        throw std::out_of_range("out of range");
    }
    for (int ind = 0; ind < degree; ind++) {
        if (corners[ind] < 0 || corners[ind] >= cur_vertex) {
            printf("vertex %d does not exist, cannot add face\n", corners[ind]);
            throw std::out_of_range("out of range");
        }
    }
    face_t * face = &faces[cur_face];
    face->degree = degree;
    face->corners = corners;

    cur_face++;
}

void const Geometry::calc_centroid() {
    float * centroid = (float *) calloc(4, sizeof(float));
    for (int ind = 0; ind < cur_vertex; ind++) {
        float * v = g_vertex(ind);
        centroid[0] += v[0];
        centroid[1] += v[1];
        centroid[2] += v[2];
    }
    centroid[0] /= cur_vertex;
    centroid[1] /= cur_vertex;
    centroid[2] /= cur_vertex;
    centroid = centroid;
}

void const Geometry::calc_bounding_radius() {
    if (centroid == NULL) {
        calc_centroid();
    }
    float * centroid = centroid;
    float radius = -1.0;
    for (int ind = 0; ind < cur_vertex; ind++) {
        float * v = g_vertex(ind);
        float dist = 0;
        for (int ind = 0; ind < DIMENSION; ind++) {
            dist += (centroid[ind] - v[ind]) * (centroid[ind] - v[ind]);
        }
        if (dist > radius) {
            radius = dist;
        }
    }
    radius = sqrt(radius);
}

void const Geometry::calc_bounding_cube() {
    cube = (float *) malloc(sizeof(float) * 6);
    float * v = g_vertex(0);
    cube[0] = v[0];
    cube[1] = v[0];
    cube[2] = v[1];
    cube[3] = v[1];
    cube[4] = v[2];
    cube[5] = v[2];
    for (int ind = 1; ind < cur_vertex; ind++) {
        float * v = g_vertex(ind);
        if (v[0] < cube[0]) {
            cube[0] = v[0];
        }
        if (v[0] > cube[1]) {
            cube[1] = v[0];
        }
        if (v[1] < cube[2]) {
            cube[2] = v[1];
        }
        if (v[1] > cube[3]) {
            cube[3] = v[1];
        }
        if (v[2] < cube[4]) {
            cube[4] = v[2];
        }
        if (v[2] > cube[5]) {
            cube[5] = v[2];
        }
    }
}

float * const Geometry::g_vertex(int ind) {
    if (ind >= cur_vertex || ind < 0) {
        std::cout << "vertex " << ind << " does not exist.\n";
        throw std::out_of_range("out of range");
    }
    return vertices + (ind * DIMENSION);
}

face_t * const Geometry::g_face(int ind) {
    if (ind >= cur_face || ind < 0) {
        std::cout << "face " << ind << " does not exist.\n";
        throw std::out_of_range("out of range");
    }

    return &faces[ind];
}

int const Geometry::g_num_vertices() {
    return num_vertices;
}

int const Geometry::g_num_faces() {
    return num_faces;
}

float * const Geometry::g_model_buffer() {
    if (cur_face < num_faces || cur_vertex < num_vertices) {
        printf("Fill up geometry before getting model buffer.\n");
        throw std::out_of_range("out of range");
    }
    int face_size = VERTEX_SIZE * 3;
    float * buffer = (float *) malloc(face_size * g_num_faces());
    for (int face_ind = 0; face_ind < g_num_faces(); face_ind++) {
        face_t * face = g_face(face_ind);
        for (int corner = 0; corner < face->degree; corner++) {
            float * vert = g_vertex(face->corners[corner]);
            // these should be equivalent!
            // memcpy(buffer + face_size * face_ind + VERTEX_SIZE * corner, vert, VERTEX_SIZE);
            buffer[face_ind * 9 + corner * 3] = vert[0];
            buffer[face_ind * 9 + corner * 3 + 1] = vert[1];
            buffer[face_ind * 9 + corner * 3 + 2] = vert[2];
        }
    }
    return buffer;
}

float * Geometry::g_centroid() {
    if (cur_face < num_faces || cur_vertex < num_vertices) {
        printf("Fill up geometry before getting centroid.\n");
        throw std::out_of_range("out of range");
    }
    if (centroid == NULL) {
        calc_centroid();
    }
    return centroid;
}

float Geometry::g_bounding_radius() {
    if (cur_face < num_faces || cur_vertex < num_vertices) {
        printf("Fill up geometry before getting radius.\n");
        throw std::out_of_range("out of range");
    }
    if (radius == -1.0) {
        calc_bounding_radius();
    }
    return radius;
}

float * Geometry::g_bounding_cube() {
    if (cur_face < num_faces || cur_vertex < num_vertices) {
        printf("Fill up geometry before getting radius.\n");
        throw std::out_of_range("out of range");
    }
    if (cube == NULL) {
        calc_bounding_cube();
    }
    return cube;
}

/**
 * Gets the transformation matrix for position, orientation, and scale
 *      pass in NULL for a paramater to get default.
 *      TODO: use orientation
 * @param state
 * @return
 */
GLfloat * Geometry::transformation_matrix(geometric_state_t * state) {
    GLfloat * translation = (GLfloat *) calloc(16, sizeof(GLfloat));
    //diagonal
    translation[0]  = state->scale[0];
    translation[5]  = state->scale[1];
    translation[10] = state->scale[2];
    translation[15] = 1.0;
    //right most column
    translation[12] = state->position[0];
    translation[13] = state->position[1];
    translation[14] = state->position[2];
    return translation;
}

GLfloat * Geometry::interp_matrix(std::vector<geometric_state_t *> * states, std::vector<float> * weights) {
    //TODO
    return NULL;
}
