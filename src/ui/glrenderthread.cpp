
#include <GL/glew.h>

#include "glframe.h"

#include "glrenderthread.moc"
#include "glrenderthread.h"
#include <iostream>
#include <QDateTime>

#include "Scene.h"
#include "WireframeScene.h"
#include "TreeScene.h"
#include "ProceduralScene.h"
#include "PerlinScene.h"
#include "EfficientScene.h"
#include "ToonScene.h"

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
}

void QGLRenderThread::run() {

    init_gl_context();
    if (doResize) {
        GLResize(w, h);
        doResize = false;
    }

    // WireframeScene scene;
    // TreeScene scene;
    // ProceduralScene scene;
    // PerlinScene scene;
    // EfficientScene scene;
    ToonScene scene;
    scene.setup_scene();

    while (doRendering) {
        long int start = QDateTime::currentMSecsSinceEpoch();
        if (doResize) {
            GLResize(w, h);
            doResize = false;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.render(w, h);

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

void QGLRenderThread::GLResize(int width, int height) {
    glViewport(0, 0, width, height);
}
