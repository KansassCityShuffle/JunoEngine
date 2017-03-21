#ifndef DETECTIONWIDGET_H
#define DETECTIONWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include "transform3d.h"

class QExposeEvent;
class QOpenGLShaderProgram;
class QOpenGLTexture;

class DetectionWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT


public:
    DetectionWidget();
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
protected slots:
    void teardownGL();
    void update();



protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    // Controls information
    bool rotateCam;
    int prevMouseX;
    int prevMouseY;
    float angleX;
    float angleY;
    float fov;

    // OpenGL State Information
    QOpenGLBuffer m_vertices;
    QOpenGLVertexArrayObject m_object;
    QOpenGLShaderProgram *m_program;
    QOpenGLTexture *m_texture;
    int m_faces;

    // Shader Information
    int u_modelToWorld;
    int u_worldToView;
    QMatrix4x4 m_projection;
    Transform3D m_transform;

    // Fix for Windows
    QRegion m_cachedRegion;

    // Private Helpers
    void printVersionInformation();
    bool loadObj(const char* path);
};

#endif // DETECTIONWIDGET_H
