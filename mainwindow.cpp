#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "detectionwidget.h"
#include "slamwidget.h"
#include <QSplitter>
#include <QKeyEvent>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Set OpenGL Version information
    // Note: This format must be set before show() is called.
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(8);
    format.setVersion(4, 5);
/*
    DetectionWidget* dw = new DetectionWidget();
    dw->setFormat(format);
    dw->resize(QSize(256, 128));
*/
    SlamWidget* sw = new SlamWidget();
    sw->setFormat(format);
    sw->resize(QSize(256, 128));
    sw->setFocusPolicy(Qt::ClickFocus);
    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Horizontal);
//    splitter->addWidget(dw);
    splitter->addWidget(sw);

    ui->verticalLayout->addWidget(splitter);

}

MainWindow::~MainWindow()
{
    delete ui;
}

