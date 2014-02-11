
#include <QFileInfo>

#include "glframe.h"

#include "glrenderthread.moc"
#include "glrenderthread.h"
#include "load_collada.h"
#include <GL/glu.h>
#include <iostream>
#include <QDateTime>

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
    GLInit();
    LoadShaders(NULL, "shaders/wire.gsh", "shaders/wire.fsh");
    suzanne = load_collada("assets/models/suzanne.dae");

    while (doRendering) {
        long int start = QDateTime::currentMSecsSinceEpoch();
        if(doResize) {
            GLResize(w, h);
            doResize = false;
        }

        paintGL(); // render actual frame

        FrameCounter++;
        GLFrame->swapBuffers();
        int time_elapsed = QDateTime::currentMSecsSinceEpoch() - start;
        // std::cout << "Frame took " << time_elapsed << " miliseconds.\n";
        if (time_elapsed < FRAME_LENGTH) {
            msleep(FRAME_LENGTH - time_elapsed);
        }
    }
}


void QGLRenderThread::GLInit(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}


void QGLRenderThread::GLResize(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(45., ((GLfloat)width)/((GLfloat)height), 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
}


void QGLRenderThread::paintGL(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(FrameCounter, 1.0f, 1.0f, 1.0f);

    for (int face_ind = 0; face_ind < suzanne->g_num_faces(); face_ind++) {
        face_t * face = suzanne->g_face(face_ind);
        glBegin(GL_POLYGON);
        for (int corner = 0; corner < face->degree; corner++) {
            float * vert = suzanne->g_vertex(face->corners[corner]);
            glVertex3f(vert[0], vert[1], vert[2]);
        }
        glEnd();
    }
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

    if (vshader == NULL) {
        vshader = "shaders/default.vsh";
    }
    if (gshader == NULL) {
        gshader = "shaders/default.gsh";
    }
    if (fshader == NULL) {
        fshader = "shaders/default.fsh";
    }
    LoadShader(QGLShader::Vertex, VertexShader, vshader);
    LoadShader(QGLShader::Geometry, GeometryShader, gshader);
    LoadShader(QGLShader::Fragment, FragmentShader, fshader);

    if (!ShaderProgram->link()) {
        qWarning() << "Shader Program Linker Error" << ShaderProgram->log();
    } else {
        ShaderProgram->bind();
    }
}
