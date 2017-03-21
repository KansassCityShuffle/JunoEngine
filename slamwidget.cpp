#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QExposeEvent>
#include <QtGlobal>
#include <QTime>

#include "slamwidget.h"
#include "vertex.h"

// Front Verticies
#define VERTEX_FTR Vertex( QVector3D( 0.5f,  0.5f,  0.5f), QVector3D( 1.0f, 0.0f, 0.0f ) )
#define VERTEX_FTL Vertex( QVector3D(-0.5f,  0.5f,  0.5f), QVector3D( 0.0f, 1.0f, 0.0f ) )
#define VERTEX_FBL Vertex( QVector3D(-0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 1.0f ) )
#define VERTEX_FBR Vertex( QVector3D( 0.5f, -0.5f,  0.5f), QVector3D( 0.0f, 0.0f, 0.0f ) )

// Back Verticies
#define VERTEX_BTR Vertex( QVector3D( 0.5f,  0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 0.0f ) )
#define VERTEX_BTL Vertex( QVector3D(-0.5f,  0.5f, -0.5f), QVector3D( 0.0f, 1.0f, 1.0f ) )
#define VERTEX_BBL Vertex( QVector3D(-0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 0.0f, 1.0f ) )
#define VERTEX_BBR Vertex( QVector3D( 0.5f, -0.5f, -0.5f), QVector3D( 1.0f, 1.0f, 1.0f ) )

// Create a colored cube
static const Vertex sg_vertexes[] = {
  // Face 1 (Front)
    VERTEX_FTR, VERTEX_FTL, VERTEX_FBL,
    VERTEX_FBL, VERTEX_FBR, VERTEX_FTR,
  // Face 2 (Back)
    VERTEX_BBR, VERTEX_BTL, VERTEX_BTR,
    VERTEX_BTL, VERTEX_BBR, VERTEX_BBL,
  // Face 3 (Top)
    VERTEX_FTR, VERTEX_BTR, VERTEX_BTL,
    VERTEX_BTL, VERTEX_FTL, VERTEX_FTR,
  // Face 4 (Bottom)
    VERTEX_FBR, VERTEX_FBL, VERTEX_BBL,
    VERTEX_BBL, VERTEX_BBR, VERTEX_FBR,
  // Face 5 (Left)
    VERTEX_FBL, VERTEX_FTL, VERTEX_BTL,
    VERTEX_FBL, VERTEX_BTL, VERTEX_BBL,
  // Face 6 (Right)
    VERTEX_FTR, VERTEX_FBR, VERTEX_BBR,
    VERTEX_BBR, VERTEX_BTR, VERTEX_FTR
};

#undef VERTEX_BBR
#undef VERTEX_BBL
#undef VERTEX_BTL
#undef VERTEX_BTR

#undef VERTEX_FBR
#undef VERTEX_FBL
#undef VERTEX_FTL
#undef VERTEX_FTR

/*******************************************************************************
 * OpenGL Events
 ******************************************************************************/

SlamWidget::SlamWidget()
{
    angleX = 0.f;
    angleY = 1.f;
    fov = 70.f;
    distance = 20.f;

    qsrand(QTime::currentTime().msec());

    for (int i = 0; i < 2048; i++)
    for (int j = 0; j < 2048; j++)
    {
        if (qrand() % 100 == 1) mapOffsets.append(QVector2D(i-1024,j-1024));
    }

}

void SlamWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (rotateCam)
    {
        float xAmt = float(event->x() - prevMouseX);
        float yAmt = float(event->y() - prevMouseY);

        angleX += xAmt * 0.01f;
        angleY += yAmt * 0.01f;

        if (angleY > 1.56f) angleY = 1.56f;
        if (angleY < -1.56f) angleY = -1.56f;

        prevMouseX = event->x();
        prevMouseY = event->y();

    }
}

void SlamWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == 2) // right button
    {
        prevMouseX = event->x();
        prevMouseY = event->y();
        rotateCam = true;
    }
}

void SlamWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == 2) // right button
    {
        prevMouseX = -1;
        prevMouseY = -1;
        rotateCam = false;
    }
}

void SlamWidget::wheelEvent(QWheelEvent *event)
{
    distance -= event->angleDelta().y() * 0.00125f * distance;
    if (distance < 2.f) distance = 2.f;
}

void SlamWidget::initializeGL()
{
  // Initialize OpenGL Backend
  initializeOpenGLFunctions();
  connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);
  connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
  printVersionInformation();

  // Set global information
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Application-specific initialization
  {
    // Create Shader (Do not release until VAO is created)
    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
    m_program->link();
    m_program->bind();

    // Cache Uniform Locations
    u_modelToWorld = m_program->uniformLocation("modelToWorld");
    u_worldToView = m_program->uniformLocation("worldToView");

    // Create Buffer (Do not release until VAO is created)
    m_vertex.create();
    m_vertex.bind();
    m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertex.allocate(sg_vertexes, sizeof(sg_vertexes));

    // Create Vertex Array Object
    m_object.create();
    m_object.bind();
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(),
                                  Vertex::PositionTupleSize, Vertex::stride());
    m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(),
                                  Vertex::ColorTupleSize, Vertex::stride());

    // Release (unbind) all
    m_object.release();
    m_vertex.release();
    m_program->release();
  }
}

void SlamWidget::resizeGL(int width, int height)
{
}

void SlamWidget::paintGL()
{

    m_projection.setToIdentity();
    m_projection.perspective(fov, this->width() / float(this->height()), 0.001f, 3000.0f);
    m_projection.lookAt(QVector3D(distance * cos(angleY) * cos(angleX),
                                  distance * sin(angleY),
                                  distance * cos(angleY)*sin(angleX)),
                        QVector3D(0.f, 0.f, 0.f),
                        QVector3D(0.f, 1.f, 0.f));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();
    m_program->setUniformValue(u_worldToView, m_projection);
    m_object.bind();

    for (QVector2D pos : mapOffsets)
    {
        m_transform.setTranslation(pos.x(), 0.0f, pos.y());
        m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
        glDrawArrays(GL_TRIANGLES, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
    }
    //glDrawArraysInstanced(GL_TRIANGLES, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]), 2000);

    m_object.release();
    m_program->release();
}

void SlamWidget::teardownGL()
{
  // Actually destroy our OpenGL information
  m_object.destroy();
  m_vertex.destroy();
  delete m_program;
}

void SlamWidget::update()
{
  // Schedule a redraw
  QOpenGLWidget::update();
}

/*******************************************************************************
 * Private Helpers
 ******************************************************************************/

void SlamWidget::printVersionInformation()
{
  QString glType;
  QString glVersion;
  QString glProfile;

  // Get Version Information
  glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

  // Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
  switch (format().profile())
  {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
  }
#undef CASE

  // qPrintable() will print our QString w/o quotes around it.
  qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}
