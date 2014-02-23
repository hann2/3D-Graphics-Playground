

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdexcept>
#include <math.h>
#include "IndexedFaceSet.h"

#define DIMENSION 3
#define VERTEX_SIZE (sizeof(float) * DIMENSION)

IndexedFaceSet::IndexedFaceSet(int n_vertices, int n_faces) {
    cur_face = 0;
    cur_vertex = 0;
    cur_tex_coord = 0;
    cur_vertex_normal = 0;
    num_vertices = n_vertices;
    num_faces = n_faces;
    vertices = (float *) malloc(num_vertices * VERTEX_SIZE);
    faces = (face_t *) calloc(num_faces, sizeof(face_t));
    tex_coords = (float *) malloc(num_vertices * 2 * sizeof(float));
    vertex_normals = NULL;
}

IndexedFaceSet::~IndexedFaceSet() {
    free(vertices);
    for (int ind = 0; ind < cur_face; ind++) {
        free(faces[ind].corners);
    }
    free(faces);
}

void IndexedFaceSet::add_vertex(float * vertex) {
    if (cur_vertex == num_vertices) {
        std::cout << "IndexedFaceSet is full, cannot add vertex.\n";
        throw std::out_of_range("out of range");
    }

    //copy vertex directly into vertices
    memcpy(vertices + (cur_vertex * DIMENSION), vertex, VERTEX_SIZE);

    cur_vertex++;
}

void IndexedFaceSet::add_face(int degree, int * corners) {
    if (cur_face == num_faces) {
        std::cout << "IndexedFaceSet is full, cannot add face.\n";
        throw std::out_of_range("out of range");
    }
    for (int ind = 0; ind < degree; ind++) {
        if (corners[ind] < 0 || corners[ind] >= cur_vertex) {
            std::cout << "vertex " << corners[ind] << " does not exist, cannot add face\n";
            std::cout << num_vertices << "\n";
            throw std::out_of_range("out of range");
        }
    }
    face_t * face = &faces[cur_face];
    face->degree = degree;
    face->corners = (int *) malloc(sizeof(int) * degree);
    memcpy(face->corners, corners, sizeof(int) * degree);

    cur_face++;
}

void IndexedFaceSet::add_texture_coordinate(float * coord) {
    memcpy(tex_coords + cur_tex_coord * 2, coord, 2 * sizeof(float));
    cur_tex_coord++;
}

void IndexedFaceSet::add_vertex_normal(float * normal) {
    if (vertex_normals == NULL) {
        vertex_normals = (float *) malloc(num_vertices * VERTEX_SIZE);
    }
    memcpy(vertex_normals + (cur_vertex_normal * DIMENSION), normal, VERTEX_SIZE);
    cur_vertex_normal++;
}

float * IndexedFaceSet::g_vertex(int ind) {
    if (ind >= cur_vertex || ind < 0) {
        std::cout << "vertex " << ind << " does not exist.\n";
        throw std::out_of_range("out of range");
    }
    return vertices + (ind * DIMENSION);
}

face_t * IndexedFaceSet::g_face(int ind) {
    if (ind >= cur_face || ind < 0) {
        std::cout << "face " << ind << " does not exist.\n";
        throw std::out_of_range("out of range");
    }

    return &faces[ind];
}

float * IndexedFaceSet::g_texture_coordinate(int ind) {
    return tex_coords + (ind * 2);
}

int IndexedFaceSet::g_num_vertices() {
    return cur_vertex;
}

int IndexedFaceSet::g_num_faces() {
    return cur_face;
}

float * IndexedFaceSet::g_vertex_buffer() {
    if (cur_face < num_faces || cur_vertex < num_vertices) {
        std::cout << "Fill up geometry before getting model buffer.\n";
        throw std::out_of_range("out of range");
    }
    float * buffer = (float *) malloc(g_vertex_buffer_size());
    memcpy(buffer, vertices, g_vertex_buffer_size());
    return buffer;
}

float * IndexedFaceSet::g_vertex_normal_buffer() {
    if (cur_face < num_faces || cur_vertex < num_vertices) {
        std::cout << "Fill up geometry before getting normal buffer.\n";
        throw std::out_of_range("out of range");
    }
    if (vertex_normals == NULL) {
        calculate_vertex_normals();
    }
    float * buffer = (float *) malloc(g_vertex_normal_buffer_size());
    memcpy(buffer, vertex_normals, g_vertex_normal_buffer_size());
    return buffer;
}

float * IndexedFaceSet::g_texture_coordinate_buffer() {
    if (cur_face < num_faces || cur_vertex < num_vertices || cur_tex_coord < num_vertices) {
        std::cout << "Fill up geometry before getting texture buffer.\n";
        throw std::out_of_range("out of range");
    }
    float * buffer = (float *) malloc(g_texture_coordinate_buffer_size());
    memcpy(buffer, tex_coords, g_texture_coordinate_buffer_size());
    return buffer;
}

int * IndexedFaceSet::g_index_buffer() {
    if (cur_face < num_faces || cur_vertex < num_vertices) {
        std::cout << "Fill up geometry before getting indices buffer.\n";
        throw std::out_of_range("out of range");
    }
    int * indices = (int *) malloc(g_index_buffer_size());
    for (int i = 0; i < num_faces; i++) {
        memcpy(indices + i * 3, g_face(i)->corners, 3 * sizeof(int));
    }
    return indices;
}

int IndexedFaceSet::g_vertex_buffer_size() {
    return num_vertices * VERTEX_SIZE;
}

int IndexedFaceSet::g_vertex_normal_buffer_size() {
    return num_vertices * VERTEX_SIZE;
}

int IndexedFaceSet::g_texture_coordinate_buffer_size() {
    return num_vertices * 2 * sizeof(float);
}

int IndexedFaceSet::g_index_buffer_size() {
    return num_faces * 3 * sizeof(int);
}

void IndexedFaceSet::normalize_vec(float * v) {
    float length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (length < 0.001) {
        std::cout << "trying to normalize zero vector\n";
    }
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
}

float * IndexedFaceSet::g_face_normal(int face_ind) {
    face_t * face = g_face(face_ind);
    float * normal = (float *) malloc(3 * sizeof(float));
    float * p0 = g_vertex(face->corners[0]);
    float * p1 = g_vertex(face->corners[1]);
    float * p2 = g_vertex(face->corners[2]);

    float ux = p2[0] - p1[0];
    float uy = p2[1] - p1[1];
    float uz = p2[2] - p1[2];

    float vx = p0[0] - p1[0];
    float vy = p0[1] - p1[1];
    float vz = p0[2] - p1[2];

    // cross product of p2 - p1 and p0 - p1
    normal[0] = uy * vz - uz * vy;
    normal[1] = uz * vx - ux * vz;
    normal[2] = ux * vy - vx * uy;

    normalize_vec(normal);

    return normal;
}

void IndexedFaceSet::calculate_vertex_normals() {
    if (vertex_normals != NULL) {
        free(vertex_normals);
    }
    vertex_normals = (float *) calloc(num_vertices * 3, sizeof(float));
    for (int face_ind = 0; face_ind < g_num_faces(); face_ind++) {
        float * face_normal = g_face_normal(face_ind);
        face_t * face = g_face(face_ind);
        for (int corner = 0; corner < face->degree; corner++) {
            vertex_normals[face->corners[corner] * 3] += face_normal[0];
            vertex_normals[face->corners[corner] * 3 + 1] += face_normal[1];
            vertex_normals[face->corners[corner] * 3 + 2] += face_normal[2];
        }
        free(face_normal);
    }

    for (int ind = 0; ind < num_vertices; ind++) {
        normalize_vec(vertex_normals + ind * 3);
    }
    cur_vertex_normal = num_vertices;
}


float * IndexedFaceSet::g_bounding_sphere() {
    if (cur_face < num_faces || cur_vertex < num_vertices) {
        std::cout << "Fill up geometry before getting bounding sphere.\n";
        throw std::out_of_range("out of range");
    }

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
    centroid[3] = sqrt(radius);
    return centroid;
}

// axis alligned homie
float * IndexedFaceSet::g_bounding_cube() {
    if (cur_face < num_faces || cur_vertex < num_vertices) {
        std::cout << "Fill up geometry before bounding cube.\n";
        throw std::out_of_range("out of range");
    }
    float * cube = (float *) malloc(sizeof(float) * 6);
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
    return cube;
}
