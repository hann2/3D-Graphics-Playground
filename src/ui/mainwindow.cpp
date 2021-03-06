#include "mainwindow.h"
#include "../ui_mainwindow.h"

#include "mainwindow.moc"
#include "glframe.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setFixedSize(1440, 810);
    ui->setupUi(this);
    GLFrame = new QGLFrame();      // create our subclassed GLWidget
    setCentralWidget(GLFrame);      // assign it to the central widget of the window
    GLFrame->initRenderThread();    // start rendering
}

MainWindow::~MainWindow()
{
    delete GLFrame;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *evt)
{
    GLFrame->stopRenderThread();    // stop the thread befor exiting
    QMainWindow::closeEvent(evt);
}
