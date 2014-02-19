#ifndef GLRENDERTHREAD_H
#define GLRENDERTHREAD_H

#include "Model.h"

#include <QThread>
#include <QGLShader>
#include <QGLWidget>
#include <glm/glm.hpp>

class QGLFrame;
class QSize;
class QGLShaderProgram;
class QGLShader;

class QGLRenderThread : public QThread
{
    Q_OBJECT
public:
    explicit QGLRenderThread(QGLFrame *parent = 0);
    void resizeViewport(const QSize &size);
    void init_gl_context();
    void run();
    void stop();
    GLuint load_shader(std::string file_name, GLenum shader_type);
    GLuint load_shaders(std::string v_file, std::string g_file, std::string f_file);

protected:
    void GLResize(int width, int height);

private:
    void load_procedural_scene();
    void load_perlin_demo();
    QTimer * timer;
    bool doRendering, doResize;
    int w, h, FrameCounter;
    std::vector<Model *> models;

    QGLFrame *GLFrame;

signals:

public slots:

};

#endif // GLRENDERTHREAD_H
