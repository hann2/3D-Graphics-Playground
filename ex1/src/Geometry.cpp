

#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdexcept>
#include <math.h>
#include "Geometry.h"

#define DIMENSION 3
#define VERTEX_SIZE (sizeof(float) * DIMENSION)

Geometry::Geometry(int num_vertices, int num_faces) {
    this->cur_face = 0;
    this->cur_vertex = 0;
    this->num_vertices = num_vertices;
    this->num_faces = num_faces;
    this->radius = -1.0;
    this->centroid = NULL;
    this->vertices = (float *) malloc(num_vertices * VERTEX_SIZE);
    this->faces = (face_t *) calloc(num_faces, sizeof(face_t));
    this->bones = NULL;
    this->weights = NULL;
}

Geometry::~Geometry() {
    free(this->vertices);
    for (int ind = 0; ind < this->cur_face; ind++) {
        free(this->faces[ind].corners);
    }
    free(this->faces);
}

void Geometry::add_vertex(float * vertex) {
    if (this->cur_vertex == this->num_vertices) {
        printf("Geometry is full, cannot add vertex.\n");
        throw std::out_of_range("out of range");
    }

    //copy vertex directly into vertices
    memcpy(this->vertices + (this->cur_vertex * DIMENSION), vertex, VERTEX_SIZE);

    this->cur_vertex++;
}

void Geometry::add_face(int degree, int * corners) {
    if (this->cur_face == this->num_faces) {
        printf("Geometry is full, cannot add face.\n");
        throw std::out_of_range("out of range");
    }
    for (int ind = 0; ind < degree; ind++) {
        if (corners[ind] < 0 || corners[ind] >= this->cur_vertex) {
            printf("vertex %d does not exist, cannot add face\n", corners[ind]);
            throw std::out_of_range("out of range");
        }
    }
    face_t * face = &this->faces[this->cur_face];
    face->degree = degree;
    face->corners = corners;

    this->cur_face++;
}

void const Geometry::calc_centroid() {
    float * centroid = (float *) calloc(4, sizeof(float));
    for (int ind = 0; ind < this->cur_vertex; ind++) {
        float * v = this->g_vertex(ind);
        centroid[0] += v[0];
        centroid[1] += v[1];
        centroid[2] += v[2];
    }
    centroid[0] /= this->cur_vertex;
    centroid[1] /= this->cur_vertex;
    centroid[2] /= this->cur_vertex;
    this->centroid = centroid;
}

void const Geometry::calc_bounding_radius() {
    if (this->centroid == NULL) {
        this->calc_centroid();
    }
    float * centroid = this->centroid;
    float radius = -1.0;
    for (int ind = 0; ind < this->cur_vertex; ind++) {
        float * v = this->g_vertex(ind);
        float dist = 0;
        for (int ind = 0; ind < DIMENSION; ind++) {
            dist += (centroid[ind] - v[ind]) * (centroid[ind] - v[ind]);
        }
        if (dist > radius) {
            radius = dist;
        }
    }
    this->radius = sqrt(radius);
}

float * const Geometry::g_vertex(int ind) {
    if (ind >= this->cur_vertex || ind < 0) {
        printf("vertex %d does not exist.\n", ind);
        throw std::out_of_range("out of range");
    }
    return this->vertices + (ind * DIMENSION);
}

face_t * const Geometry::g_face(int ind) {
    if (ind >= this->cur_face || ind < 0) {
        printf("face %d does not exist.\n", ind);
        throw std::out_of_range("out of range");
    }

    return &this->faces[ind];
}

int const Geometry::g_num_vertices() {
    return this->num_vertices;
}

int const Geometry::g_num_faces() {
    return this->num_faces;
}

float * Geometry::g_centroid() {
    if (this->cur_face < this->num_faces || this->cur_vertex < this->num_vertices) {
        printf("Fill up geometry before getting centroid.\n");
        throw std::out_of_range("out of range");
    }
    if (this->centroid == NULL) {
        this->calc_centroid();
    }
    return this->centroid;
}

float Geometry::g_bounding_radius() {
    if (this->cur_face < this->num_faces || this->cur_vertex < this->num_vertices) {
        printf("Fill up geometry before getting radius.\n");
        throw std::out_of_range("out of range");
    }
    if (this->radius == -1.0) {
        this->calc_bounding_radius();
    }
    return this->radius;
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
