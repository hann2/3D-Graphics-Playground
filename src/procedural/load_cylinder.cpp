
#include "load_cylinder.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>

#define PI 3.141592

IndexedFaceSet * load_cylinder(int subdivisions) {
    IndexedFaceSet * cylinder = new IndexedFaceSet();
    float * vertex = (float *) malloc(sizeof(float) * 3);
    float * tex_coord = (float *) malloc(sizeof(float) * 2);
    for (int i = 0; i < subdivisions; i++) {
        float angle = i * 2 * PI / subdivisions;
        vertex[0] = cos(angle);
        vertex[1] = sin(angle);
        vertex[2] = 0;
        cylinder->add_vertex(vertex);
        vertex[2] = 1.0;
        cylinder->add_vertex(vertex);

        vertex[2] = 0.0;
        cylinder->add_vertex_normal(vertex);
        cylinder->add_vertex_normal(vertex);

        tex_coord[0] = i / (subdivisions - 1.0);
        tex_coord[1] = 0.0;
        cylinder->add_texture_coordinate(tex_coord);
        tex_coord[1] = 1.0;
        cylinder->add_texture_coordinate(tex_coord);
    }
    free(vertex);
    free(tex_coord);

    int * corners = (int *) malloc(sizeof(int) * 3);
    for (int i = 0; i < subdivisions; i++) {
        // indices swapped to ensure ccw faces
        corners[1] = i*2;
        corners[0] = i*2 + 1;
        corners[2] = (i*2 + 2) % (subdivisions * 2);
        cylinder->add_face(3, corners);
        corners[0] = i*2 + 1;
        corners[1] = (i*2 + 2) % (subdivisions * 2);
        corners[2] = (i*2 + 3) % (subdivisions * 2);
        cylinder->add_face(3, corners);
    }
    free(corners);
    return cylinder;
}

