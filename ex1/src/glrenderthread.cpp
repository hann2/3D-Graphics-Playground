
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

    ShaderProgram = NULL;
    VertexShader = GeometryShader = FragmentShader = NULL;
}

void QGLRenderThread::resizeViewport(const QSize &size) {
    w = size.width();
    h = size.height();
    doResize = true;
}

void QGLRenderThread::stop() {
    doRendering = false;
}

void QGLRenderThread::run() {
    GLFrame->makeCurrent();

    glewExperimental = GL_TRUE; 
    glewInit();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    view_transform = glm::lookAt(glm::vec3(5,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0));
    model_transform = glm::mat4(1.0f);

    Geometry * suzanne = load_collada("assets/models/suzanne.dae");
    LoadShaders("shaders/textured.vsh", NULL, "shaders/textured.fsh");

    int model_size = sizeof(float) * 3 * 3 * suzanne->g_num_faces();
    float * model = suzanne->g_model_buffer();

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, model_size, model, GL_STATIC_DRAW);

    int texture_size = sizeof(float) * 2 * 3 * suzanne->g_num_faces();
    float * texture_coords = (float *) malloc(texture_size);
    for (int face_ind = 0; face_ind < suzanne->g_num_faces(); face_ind++) {
        for (int corner = 0; corner < 3; corner++) {
            texture_coords[face_ind * 6 + corner * 2] = model[face_ind * 9 + corner * 3];
            texture_coords[face_ind * 6 + corner * 2 + 1] = model[face_ind * 9 + corner * 3 + 2];
        }
    }

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, texture_size, texture_coords, GL_STATIC_DRAW);

    GLuint tex;
    float * noise = grass_texture();
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 256, 256, 0, GL_RED, GL_FLOAT, noise);
    glGenerateMipmap(GL_TEXTURE_2D);

    GLuint texture_id  = glGetUniformLocation(ShaderProgram->programId(), "texture_sampler");
    GLuint matrix_id = glGetUniformLocation(ShaderProgram->programId(), "MVP");

    while (doRendering) {
        long int start = QDateTime::currentMSecsSinceEpoch();
        if (doResize) {
            GLResize(w, h);
            doResize = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        model_transform = glm::rotate(glm::mat4(1.0f), (float)FrameCounter, glm::vec3(1.0f));
        glm::mat4 MVP = projection_transform * view_transform * model_transform;
        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &MVP[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(texture_id, 0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

        glDrawArrays(GL_TRIANGLES, 0, model_size);
        glDisableVertexAttribArray(0);

        FrameCounter++;
        GLFrame->swapBuffers();

        if (glGetError() != GL_NO_ERROR) {
            std::cout << "Error!\n";
        }

        int time_elapsed = QDateTime::currentMSecsSinceEpoch() - start;
        // std::cout << "Frame took " << time_elapsed << " miliseconds.\n";
        if (time_elapsed < FRAME_LENGTH) {
            msleep(FRAME_LENGTH - time_elapsed);
        }
    }
}


void QGLRenderThread::GLResize(int width, int height) {
    glViewport(0, 0, width, height);
    projection_transform = glm::perspective(45.0f, ((float)width) / height, 0.1f, 1000.0f);
}

void QGLRenderThread::ClearShader(QGLShader * shader) {
    if (shader) {
        delete shader;
        shader = NULL;
    }
}

void QGLRenderThread::LoadShader(QGLShader::ShaderTypeBit shader_type, QGLShader * shader, QString file_name) {
    QFileInfo vsh(file_name);
    if (vsh.exists()) {
        shader = new QGLShader(shader_type);
        if (shader->compileSourceFile(file_name)) {
            ShaderProgram->addShader(shader);
        }
        else {
            qWarning() << "Shader Error with " << file_name << ",\n" << shader->log();
        }
    } else {
        qWarning() << "Shader source file " << file_name << " not found.";
    }
}

void QGLRenderThread::LoadShaders(QString vshader, QString gshader, QString fshader) {
    if (ShaderProgram) {
        ShaderProgram->release();
        ShaderProgram->removeAllShaders();
    } else {
        ShaderProgram = new QGLShaderProgram;
    }

    ClearShader(VertexShader);
    ClearShader(GeometryShader);
    ClearShader(FragmentShader);

    if (vshader != NULL) {
        LoadShader(QGLShader::Vertex, VertexShader, vshader);
    }
    if (gshader != NULL) {
        LoadShader(QGLShader::Geometry, GeometryShader, gshader);
    }
    if (fshader != NULL) {
        LoadShader(QGLShader::Fragment, FragmentShader, fshader);
    }
    

    if (!ShaderProgram->link()) {
        qWarning() << "Shader Program Linker Error" << ShaderProgram->log();
    } else {
        ShaderProgram->bind();
    }
}
