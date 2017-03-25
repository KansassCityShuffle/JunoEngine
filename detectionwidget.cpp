#include "detectionwidget.h"

#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QExposeEvent>
#include "vertexuv.h"

/*******************************************************************************
 * OpenGL Events
 ******************************************************************************/

DetectionWidget::DetectionWidget()
{
    angleX = -1.57f;
    angleY = 0.f;
    fov = 70.f;
}

void DetectionWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (rotateCam)
    {
        float xAmt = float(event->x() - prevMouseX);
        float yAmt = float(event->y() - prevMouseY);

        angleX += xAmt * 0.01f;
        angleY -= yAmt * 0.01f;

        if (angleY > 1.5f) angleY = 1.5f;
        if (angleY < -1.5f) angleY = -1.5f;

        prevMouseX = event->x();
        prevMouseY = event->y();

    }
}

void DetectionWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == 2) // right button
    {
        prevMouseX = event->x();
        prevMouseY = event->y();
        rotateCam = true;
    }
}

void DetectionWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == 2) // right button
    {
        prevMouseX = -1;
        prevMouseY = -1;
        rotateCam = false;
    }
}

void DetectionWidget::wheelEvent(QWheelEvent *event)
{
    fov -= event->angleDelta().y() * 0.05f;
    if (fov > 160.f) fov = 160.f;
    if (fov < 20.f) fov = 20.f;
}


void DetectionWidget::initializeGL()
{
  // Initialize OpenGL Backend
  initializeOpenGLFunctions();
  connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);
  connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
  printVersionInformation();

  // Set global information
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Application-specific initialization
  {
    m_texture = new QOpenGLTexture(QImage(QString(":/textures/proj.png")).mirrored());

    // Create Shader (Do not release until VAO is created)
    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/det.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/det.frag");
    m_program->link();
    m_program->bind();
    m_program->setUniformValue("texture", 0);

    // Cache Uniform Locations
    u_modelToWorld = m_program->uniformLocation("modelToWorld");
    u_worldToView = m_program->uniformLocation("worldToView");

    // Create Buffer (Do not release until VAO is created)
    m_vertices.create();
    m_vertices.bind();
    m_vertices.setUsagePattern(QOpenGLBuffer::StaticDraw);
    loadObj("C:\\source\\JunoEngine\\models\\project_sphere.obj");

    // Create Vertex Array Object
    m_object.create();
    m_object.bind();
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(0, GL_FLOAT, VertexUV::positionOffset(), VertexUV::PositionTupleSize, VertexUV::stride());
    m_program->setAttributeBuffer(1, GL_FLOAT, VertexUV::uvOffset(), VertexUV::UvTupleSize, VertexUV::stride());

    // Release (unbind) all
    m_object.release();
    m_vertices.release();
    m_program->release();

  }
}

void DetectionWidget::resizeGL(int width, int height)
{
}

void DetectionWidget::paintGL()
{

  m_projection.setToIdentity();
  m_projection.perspective(fov, this->width() / float(this->height()), 0.0f, 1000.0f);
  m_projection.lookAt(QVector3D(0.f, 0.f, 0.f),
                      QVector3D(cos(angleY) * cos(angleX), sin(angleY), cos(angleY)*sin(angleX)),
                      QVector3D(0.f, 1.f, 0.f));

  // Render using our shader
  m_program->bind();
  m_program->setUniformValue(u_worldToView, m_projection);
  {
    m_object.bind();
    m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());

    // Clear
    glClear(GL_COLOR_BUFFER_BIT);

    m_transform.setTranslation( 0.f,  0.f,  0.f);
    m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
    m_texture->bind();
    glDrawArrays(GL_TRIANGLES, 0, m_faces);

    m_object.release();
  }
  m_program->release();
}

void DetectionWidget::teardownGL()
{
  // Actually destroy our OpenGL information
  m_object.destroy();
  m_vertices.destroy();
  delete m_program;
  delete m_texture;
}

void DetectionWidget::update()
{
  // Update instance information
//  m_transform.rotate(0.05f, QVector3D(0.f, -1.f, 0.f));

  // Schedule a redraw
  QOpenGLWidget::update();
}

/*******************************************************************************
 * Private Helpers
 ******************************************************************************/

void DetectionWidget::printVersionInformation()
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

#include <cstdio>
#include <cstdlib>

bool DetectionWidget::loadObj(const char* path)
{
    QVector<QVector3D> vertices;
    QVector<QVector3D> out_vertices;
    QVector<QVector2D> uvs;
    QVector<QVector2D> out_uvs;
    QVector<QVector3D> normals;
    QVector<QVector3D> out_normals;
    QVector<unsigned int> vertexIndices;
    QVector<unsigned int> uvIndices;
    QVector<unsigned int> normalIndices;

    FILE * file = fopen(path, "r");
    if( file == NULL )
    {
        printf("Unable to open file %s\n", path);
        return false;
    }
    while( 1 )
    {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;

        if ( strcmp( lineHeader, "v" ) == 0 )
        {
            float vx, vy, vz;
            fscanf(file, "%f %f %f\n", &vx, &vy, &vz );
            vertices.push_back(QVector3D(vx, vy, vz));
        }
        else if ( strcmp( lineHeader, "vt" ) == 0 )
        {
            float vx, vy;
            fscanf(file, "%f %f\n", &vx, &vy );
            uvs.push_back(QVector2D(vx, vy));
        }
        else if ( strcmp( lineHeader, "vn" ) == 0 )
        {
            float vx, vy, vz;
            fscanf(file, "%f %f %f\n", &vx, &vy, &vz );
            normals.push_back(QVector3D(vx, vy, vz));
        }
        else if ( strcmp( lineHeader, "f" ) == 0 )
        {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9)
            {
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    for( unsigned int i=0; i<vertexIndices.size(); i++ )
    {
        unsigned int vertexIndex = vertexIndices[i];
        QVector3D vertex = vertices[ vertexIndex-1 ];
        out_vertices.push_back(vertex);
    }
    for( unsigned int i=0; i<uvIndices.size(); i++ )
    {
        unsigned int uvIndex = uvIndices[i];
        QVector2D uv = uvs[ uvIndex-1 ];
        out_uvs.push_back(uv);
    }

    QVector<VertexUV> vertData;
    for (int i = 0; i < out_vertices.size(); i++)
    {
        vertData.append(VertexUV(out_vertices.at(i), out_uvs.at(i)));
    }
    m_faces = vertData.size();
    m_vertices.allocate(vertData.constData(), vertData.size() * VertexUV::stride());

    return true;
}
