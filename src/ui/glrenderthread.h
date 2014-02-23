#ifndef GLRENDERTHREAD_H
#define GLRENDERTHREAD_H

#include <QThread>
#include <QGLShader>
#include <QGLWidget>
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

protected:
    void GLResize(int width, int height);

private:
    QTimer * timer;
    bool doRendering, doResize;
    int w, h, FrameCounter;

    QGLFrame *GLFrame;

signals:

public slots:

};

#endif // GLRENDERTHREAD_H
