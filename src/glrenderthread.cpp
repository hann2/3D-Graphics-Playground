
#include <GL/glew.h>

#include <QFileInfo>

#include "glframe.h"

#include "glrenderthread.moc"
#include "glrenderthread.h"
#include "load_collada.h"
#include <iostream>
#include <QDateTime>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include "ProceduralGenerator.h"
#include <string>
#include <fstream>
#include <streambuf>

// fps in hz
#define FRAME_RATE 60
// frame length in ms
#define FRAME_LENGTH (1000 / FRAME_RATE)


QGLRenderThread::QGLRenderThread(QGLFrame *parent) :
    QThread(),
    GLFrame(parent) {
    doRendering = true;
    doResize = false;
    FrameCounter = 0;
}

void QGLRenderThread::resizeViewport(const QSize &size) {
    w = size.width();
    h = size.height();
    doResize = true;
}

void QGLRenderThread::stop() {
    doRendering = false;
}

void QGLRenderThread::init_gl_context() {
    GLFrame->makeCurrent();

    glewExperimental = GL_TRUE; 
    glewInit();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    models = std::vector<Model *>();
}

void QGLRenderThread::run() {

    init_gl_context();
    load_procedural_scene();

    while (doRendering) {
        long int start = QDateTime::currentMSecsSinceEpoch();
        if (doResize) {
            GLResize(w, h);
            doResize = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (Model * model : models) {
            model->render_model();
        }

        FrameCounter++;
        GLFrame->swapBuffers();

        GLenum errCode;
        if ((errCode = glGetError()) != GL_NO_ERROR) {
            std::cout << "OpenGL error!\n";
            std::cout << gluErrorString(errCode) << "\n";
            doRendering = false;
        }

        int time_elapsed = QDateTime::currentMSecsSinceEpoch() - start;
        // std::cout << "Frame took " << time_elapsed << " miliseconds.\n";
        if (time_elapsed < FRAME_LENGTH) {
            msleep(FRAME_LENGTH - time_elapsed);
        }
    }
}

void QGLRenderThread::load_procedural_scene() {
    GLint grass_shader = load_shaders("shaders/grass.vsh", "", "shaders/grass.fsh");
    Model * terrain_model = Model::create_model(grass_shader);

    int mesh_size = 64;
    float * terrain = terrain_height(mesh_size, mesh_size);
    float tex_step = 1.0 / (mesh_size - 1);

    Geometry geometry(mesh_size * mesh_size, (mesh_size - 1) * (mesh_size - 1) * 2);

    for (int i = 0; i < mesh_size; i++) {
        for (int j = 0; j < mesh_size; j++) {
            float vertex [] = {
                (float)i,
                terrain[i * mesh_size + j] * 20,
                (float)j
            };
            float texture_coord [] = {
                i * tex_step,
                j * tex_step
            };
            geometry.add_vertex(&vertex[0]);
            geometry.add_tex_coord(geometry.g_num_vertices() - 1, &texture_coord[0]);
        }
    }

    for (int i = 0; i < mesh_size - 1; i++) {
        for (int j = 0; j < mesh_size - 1; j++) {
            int face1 [] = {
                i * mesh_size + j,
                i * mesh_size + j + 1,
                (i + 1) * mesh_size + j + 1
            };
            int face2 [] = {
                (i + 1) * mesh_size + j + 1,
                (i + 1) * mesh_size + j,
                i * mesh_size + j
            };
            geometry.add_face(3, &face1[0]);
            geometry.add_face(3, &face2[0]);
        }
    }

    float * terrain_mesh = geometry.g_model_buffer();
    float * terrain_normals = geometry.g_normals_buffer();
    float * terrain_tex_coords = geometry.g_tex_buffer();
    int terrain_mesh_size = geometry.g_model_buffer_size();
    int terrain_normals_size = geometry.g_normals_buffer_size();
    int terrain_tex_buffer_size = geometry.g_tex_buffer_size();

    terrain_model->add_attribute(terrain_mesh, terrain_mesh_size, 3, "vertex_position");
    terrain_model->add_attribute(terrain_normals, terrain_normals_size, 3, "vertex_normal");
    terrain_model->add_attribute(terrain_tex_coords, terrain_tex_buffer_size, 2, "vertex_UV");

    float * grass_text = grass_texture(256, 256);
    terrain_model->add_texture(grass_text, 256, 256, GL_FLOAT, 1, "texture_sampler");

    // glm::vec3 suzanne_location = glm::vec3(32.0f, terrain[32 * mesh_size + 30] * 20 + 1.0f, 30.0f);
    glm::mat4 view_transform = glm::lookAt(glm::vec3(-20,60,-20), glm::vec3(32, 0, 32), glm::vec3(0,1,0));
    glm::mat4 projection_transform = glm::perspective(45.0f, ((float)w) / h, 0.1f, 1000.0f);
    glm::mat4 model_transform = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 mvp = projection_transform * view_transform * model_transform;
    // put matrix in the heap so it doesnt get deallocated
    GLfloat * copied_data = (GLfloat *) malloc(sizeof(float) * 16);
    memcpy(copied_data, &mvp[0][0], sizeof(float) * 16);
    terrain_model->add_uniform_matrix("MVP", copied_data);
    models.push_back(terrain_model);

    free(terrain_mesh);
    free(terrain_normals);
    free(terrain_tex_coords);
    free(grass_text);

    // GLint wireframe_shader = load_shaders("shaders/default.vsh", "shaders/wire.gsh", "shaders/wire.fsh");
    // Model * suzanne_model = Model::create_model(wireframe_shader);
    // Geometry * suzanne_geometry = load_collada("assets/models/suzanne.dae");

    // float * suzanne_mesh = suzanne_geometry->g_model_buffer();
    // suzanne_model->add_attribute(suzanne_mesh, suzanne_geometry->g_model_buffer_size(), 3, "vertex_position");
    // model_transform = glm::translate(glm::mat4(), suzanne_location);
    // mvp = projection_transform * view_transform * model_transform;
    
    // copied_data = (GLfloat *) malloc(sizeof(float) * 16);
    // memcpy(copied_data, &mvp[0][0], sizeof(float) * 16);
    // suzanne_model->add_uniform_matrix("MVP", copied_data);
    // models.push_back(suzanne_model);

    // free(suzanne_mesh);
    // delete suzanne_geometry;
}

void QGLRenderThread::load_perlin_demo() {
    GLint topo_shader = load_shaders("shaders/textured.vsh", "", "shaders/topo.fsh");
    GLint grass_shader = load_shaders("shaders/textured.vsh", "", "shaders/grass.fsh");
    GLint no_shader = load_shaders("shaders/textured.vsh", "", "shaders/topo.fsh");
    float g_vertex_buffer_data[] = { 
        -1.0f, -1.0f, 0.0f,
        1.0f,  -1.0f, 0.0f,
        1.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  0.0f,
        -1.0f, 1.0f,  0.0f,
        -1.0f, -1.0f, 0.0f
    };
    float g_uv_buffer_data[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f
    };

    glm::mat4 view_transform = glm::lookAt(glm::vec3(4,4,10), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 projection_transform = glm::perspective(45.0f, ((float)w) / h, 0.1f, 1000.0f);

    float * empty = (float *) calloc(256 * 256, sizeof(float));
    float * texture_data [][3] = {
        {empty,                   empty, bark_test(256, 256)},
        {empty, grass_texture(256, 256), bark_texture(256, 256)},
        {empty,                   empty, terrain_height(256, 256)}
    };
    GLint shaders [][3] = {
        {no_shader,    no_shader, no_shader},
        {no_shader, grass_shader, no_shader},
        {no_shader,    no_shader, topo_shader}
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Model * model = Model::create_model(shaders[i][j]);
            model->add_attribute(&g_vertex_buffer_data[0], sizeof(g_vertex_buffer_data), 3, "vertex_position");
            model->add_attribute(&g_uv_buffer_data[0], sizeof(g_uv_buffer_data), 2, "vertex_UV");
            model->add_texture(texture_data[i][j], 256, 256, GL_FLOAT, 1, "texture_sampler");

            glm::mat4 model_transform = glm::translate(glm::mat4(), glm::vec3((i - 1) * 2.4f, (j - 1) * 2.4f, 0.0f));
            glm::mat4 mvp = projection_transform * view_transform * model_transform;
            // put matrix in the heap so it doesnt get deallocated
            GLfloat * copied_data = (GLfloat *) malloc(sizeof(float) * 16);
            memcpy(copied_data, &mvp[0][0], sizeof(float) * 16);
            model->add_uniform_matrix("MVP", copied_data);
            models.push_back(model);
        }
    }
}

void QGLRenderThread::GLResize(int width, int height) {
    glViewport(0, 0, width, height);
}


std::string read_file(std::string file_name) {
    std::ifstream t(file_name);
    std::string str;

    t.seekg(0, std::ios::end);   
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    return str;
}

GLuint QGLRenderThread::load_shader(std::string file_name, GLenum shader_type) {
    GLuint id = glCreateShader(shader_type);
    std::string shader_code = read_file(file_name);

    char const * source_pointer = shader_code.c_str();
    glShaderSource(id, 1, &source_pointer , NULL);
    glCompileShader(id);

    int log_length;
    GLint result = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_length);
    if ( log_length > 0 ){
        char * error_message = (char *) calloc(log_length + 1, sizeof(char));
        glGetShaderInfoLog(id, log_length, NULL, error_message);
        printf("%s\n", error_message);
        free(error_message);
    }

    return id;
}

GLuint QGLRenderThread::load_shaders(std::string v_file, std::string g_file, std::string f_file) {
    GLuint program_id = glCreateProgram();

    GLuint v_id = load_shader(v_file, GL_VERTEX_SHADER);
    glAttachShader(program_id, v_id);
    GLuint g_id;
    if (g_file != "") {
        g_id = load_shader(g_file, GL_GEOMETRY_SHADER);
        glAttachShader(program_id, g_id);
    }
    GLuint f_id = load_shader(f_file, GL_FRAGMENT_SHADER);
    glAttachShader(program_id, f_id);

    glLinkProgram(program_id);

    int log_length;
    GLint result = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
    if ( log_length > 0 ){
        char * error_message = (char *) calloc(log_length + 1, sizeof(char));
        glGetProgramInfoLog(program_id, log_length, NULL, error_message);
        printf("%s\n", error_message);
        free(error_message);
    }

    glDeleteShader(v_id);
    if (g_file != "") {
        glDeleteShader(g_id);
    }
    glDeleteShader(f_id);

    return program_id;
}
