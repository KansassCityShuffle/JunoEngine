#ifndef SLAMWIDGET_H
#define SLAMWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector>
#include <QHash>

#include "transform3d.h"

class QExposeEvent;
class QOpenGLShaderProgram;

class SlamWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_0_Core
{
    Q_OBJECT

public:
    SlamWidget();
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
    void keyPressEvent(QKeyEvent* event);
    QVector3D robot_position;
    QVector4D robot_orientation;

private:
    bool rotateCam;
    int prevMouseX;
    int prevMouseY;
    float angleX;
    float angleY;
    float fov;
    float distance;
    QVector<QVector2D> mapOffsets;

    // OpenGL State Information
    QOpenGLBuffer m_vertex;
    QOpenGLVertexArrayObject m_object;
    QOpenGLShaderProgram *m_program;

    // Shader Information
    int u_modelToWorld;
    int u_worldToView;
    QMatrix4x4 m_projection;
    Transform3D m_transform;

    // Fix for Windows
    QRegion m_cachedRegion;

    // Private Helpers
    void printVersionInformation();
};

#endif // SLAMWIDGET_H
