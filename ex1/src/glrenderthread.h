#ifndef GLRENDERTHREAD_H
#define GLRENDERTHREAD_H

#include "Geometry.h"
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
    void run(void);
    void stop(void);
    void ClearShader(QGLShader * shader);
    void LoadShader(QGLShader::ShaderTypeBit shader_type, QGLShader * shader, QString file_name);
    void LoadShaders(QString vshader, QString gshader, QString fshader);
    void LoadShaders(QString vshader, QString fshader);
    void LoadShaders();

protected:
    void GLInit(void);
    void GLResize(int width, int height);

private:
    glm::mat4 model_transform;
    glm::mat4 view_transform;
    glm::mat4 projection_transform;
    QTimer * timer;
    bool doRendering, doResize;
    int w, h, FrameCounter;

    QGLFrame *GLFrame;

    QGLShaderProgram *ShaderProgram;
    QGLShader *VertexShader, *GeometryShader, *FragmentShader;

signals:

public slots:

};

#endif // GLRENDERTHREAD_H
