

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdexcept>
#include <math.h>
#include "IndexedFaceSet.h"

IndexedFaceSet::IndexedFaceSet() {
    vertices = std::vector<float>();
    tex_coords = std::vector<float>();
    vertex_normals = std::vector<float>();
    faces = std::vector<face_t>();
    num_triangles = 0;
}

IndexedFaceSet::~IndexedFaceSet() {

}

void IndexedFaceSet::add_vertex(float * v) {
    vertices.push_back(v[0]);
    vertices.push_back(v[1]);
    vertices.push_back(v[2]);
}

void IndexedFaceSet::add_face(int degree, int * corners){
    for (int ind = 0; ind < degree; ind++) {
        if (corners[ind] < 0 || corners[ind] * 3 >= (int) vertices.size()) {
            std::cout << "vertex " << corners[ind] << " does not exist, cannot add face\n";
            throw std::out_of_range("out of range");
        }
    }
    face_t face = {
        .degree = degree,
        .corners = (int *) malloc(sizeof(int) * degree)
    };
    memcpy(face.corners, corners, sizeof(int) * degree);
    faces.push_back(face);

    num_triangles += degree - 2;
}

void IndexedFaceSet::add_texture_coordinate(float * coord) {
    tex_coords.push_back(coord[0]);
    tex_coords.push_back(coord[1]);
}

void IndexedFaceSet::add_vertex_normal(float * normal) {
    vertex_normals.push_back(normal[0]);
    vertex_normals.push_back(normal[1]);
    vertex_normals.push_back(normal[2]);
}

float * IndexedFaceSet::g_vertex(int ind) {
    return &vertices[ind * 3];
}

face_t * IndexedFaceSet::g_face(int ind) {
    return &faces[ind];
}

float * IndexedFaceSet::g_texture_coordinate(int ind) {
    return &tex_coords[ind * 2];
}

float * IndexedFaceSet::g_vertex_normal(int ind) {
    return &vertex_normals[ind * 3];
}

int IndexedFaceSet::g_num_vertices() {
    return vertices.size() / 3;
}

int IndexedFaceSet::g_num_faces() {
    return faces.size();
}

float * IndexedFaceSet::g_vertex_buffer() {
    float * buffer = (float *) malloc(g_vertex_buffer_size());
    std::copy(vertices.begin(), vertices.end(), buffer);
    return buffer;
}

float * IndexedFaceSet::g_vertex_normal_buffer() {
    if (vertex_normals.size() == 0) {
        calculate_vertex_normals();
    }
    float * buffer = (float *) malloc(g_vertex_normal_buffer_size());
    std::copy(vertex_normals.begin(), vertex_normals.end(), buffer);
    return buffer;
}

float * IndexedFaceSet::g_texture_coordinate_buffer() {
    float * buffer = (float *) malloc(g_texture_coordinate_buffer_size());
    std::copy(tex_coords.begin(), tex_coords.end(), buffer);
    return buffer;
}

int * IndexedFaceSet::g_index_buffer() {
    // todo, make this work with non triangles
    int * indices = (int *) malloc(g_index_buffer_size());
    for (int i = 0; i < g_num_faces(); i++) {
        memcpy(indices + i * 3, g_face(i)->corners, 3 * sizeof(int));
    }
    return indices;
}

int IndexedFaceSet::g_vertex_buffer_size() {
    return vertices.size() * sizeof(float);
}

int IndexedFaceSet::g_vertex_normal_buffer_size() {
    return vertex_normals.size() * sizeof(float);
}

int IndexedFaceSet::g_texture_coordinate_buffer_size() {
    return tex_coords.size() * sizeof(float);
}

int IndexedFaceSet::g_index_buffer_size() {
    return num_triangles * sizeof(int) * 3;
}

void IndexedFaceSet::normalize_vec(float * v) {
    float length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (length < 0.0001) {
        std::cout << "trying to normalize near singular vector. length: " << length << "\n";
        return;
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
    vertex_normals.resize(vertices.size());
    std::fill(vertex_normals.begin(), vertex_normals.end(), 0.0f);

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

    for (int ind = 0; ind < g_num_vertices(); ind++) {
        normalize_vec(&vertex_normals[ind * 3]);
    }
}


float * IndexedFaceSet::g_bounding_sphere() {
    float * centroid = (float *) calloc(4, sizeof(float));
    for (int ind = 0; ind < g_num_vertices(); ind++) {
        float * v = g_vertex(ind);
        centroid[0] += v[0];
        centroid[1] += v[1];
        centroid[2] += v[2];
    }
    centroid[0] /= g_num_vertices();
    centroid[1] /= g_num_vertices();
    centroid[2] /= g_num_vertices();

    float radius = -1.0;
    for (int ind = 0; ind < g_num_vertices(); ind++) {
        float * v = g_vertex(ind);
        float dist = 0;
        for (int ind = 0; ind < 3; ind++) {
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
    float * cube = (float *) malloc(sizeof(float) * 6);
    float * v = g_vertex(0);
    cube[0] = v[0];
    cube[1] = v[0];
    cube[2] = v[1];
    cube[3] = v[1];
    cube[4] = v[2];
    cube[5] = v[2];
    for (int ind = 1; ind < g_num_vertices(); ind++) {
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
