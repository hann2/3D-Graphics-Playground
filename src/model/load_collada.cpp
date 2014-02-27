
#include <iostream>
#include <fstream>
#include <stddef.h>
#include <string>
#include <cerrno>
#include "load_collada.h"
#include "rapidxml/rapidxml.hpp"

static char * get_file_contents(const char * file_name) {
    std::ifstream t;
    t.open(file_name);
    if (!t.is_open()) {
        std::cout << "could not find file " << file_name << "\n";
        throw(errno);
    }
    t.seekg(0, std::ios::end);
    int length = t.tellg();
    t.seekg(0, std::ios::beg);
    char * buffer = (char *) malloc(sizeof(char) * (length + 1));
    t.read(buffer, length);
    t.close();

    buffer[length] = '\0';
    return buffer;
}

static float * get_floats(char * & str, int num_floats) {
    float * d = (float *) malloc(sizeof(float) * num_floats);
    int cur = 0;
    char * last = str;
    while (*str && cur < num_floats) {
        if (*str == ' ') {
            *str = '\0';
            d[cur++] = atof(last);
            *str = ' ';
            last = str + 1;
        }
        str++;
    }
    if (cur < num_floats) {
        d[cur++] = atof(last);
    }
    assert(cur == num_floats);
    return d;
}

static int * get_ints(char * & str, int num_ints) {
    int * d = (int *) malloc(sizeof(int) * num_ints);
    int cur = 0;
    char * last = str;
    while (*str && cur < num_ints) {
        if (*str == ' ') {
            *str = '\0';
            d[cur++] = atoi(last);
            *str = ' ';
            last = str + 1;
        }
        str++;
    }
    if (cur < num_ints) {
        d[cur++] = atoi(last);
    }
    assert(cur == num_ints);
    return d;
}

IndexedFaceSet * load_collada(const char * file_name) {
    char * text = get_file_contents(file_name);

    rapidxml::xml_document<> * doc = new rapidxml::xml_document<>();
    doc->parse<0>(text);

    rapidxml::xml_node<> * mesh = doc
        ->first_node("COLLADA")
        ->first_node("library_geometries")
        ->first_node("geometry")
        ->first_node("mesh");

    rapidxml::xml_node<> * vertices = mesh
        ->first_node("source");
    // rapidxml::xml_node<> * vertex_normals = vertices
    //     ->next_sibling("source")
    //     ->first_node("float_array");
    vertices = vertices->first_node("float_array");

    rapidxml::xml_node<> * faces = mesh
        ->first_node("polylist");

    int num_faces = atoi(faces->first_attribute("count")->value());

    rapidxml::xml_node<> * degrees_data = faces
        ->first_node("vcount");

    rapidxml::xml_node<> * corners_data = faces
        ->first_node("p");


    int num_vertices = atoi(vertices->first_attribute("count")->value()) / 3;

    IndexedFaceSet * model = new IndexedFaceSet();

    char * vert_ptr = vertices->value();

    for (int ind = 0; ind < num_vertices; ind += 1) {
        float * vertex = get_floats(vert_ptr, 3);
        // printf("vertex : (%lf, %lf, %lf)\n", vertex[0], vertex[1], vertex[2]);
        model->add_vertex(vertex);
        free(vertex);
    }

    char * degree_ptr = degrees_data->value();
    char * corner_ptr = corners_data->value();

    for (int ind = 0; ind < num_faces; ind += 1) {
        int * temp = get_ints(degree_ptr, 1);
        int degree = *temp;
        free(temp);
        // format is corner face_ind corner face_ind ...
        int * corners = get_ints(corner_ptr, degree * 2);
        for (int corner = 1; corner < degree; corner++) {
            corners[corner] = corners[corner*2];
        }
        corners = (int *)realloc((void *) corners, sizeof(int) * degree);
        model->add_face(degree, corners);
    }

    delete doc;
    free(text);
    return model;
}

