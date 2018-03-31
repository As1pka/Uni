#ifndef CAMERA3D_H
#define CAMERA3D_H

#include <QVector>
#include <QQuaternion>
#include <QMatrix4x4>
#include "transformational.h"

class Camera3D : public Transformational
{
private:
    QQuaternion m_rotate;
    QVector3D m_translate;
    float m_scale;
    QMatrix4x4 m_globalTransform;
    void updateViewMatrix();

public:
    Camera3D();
    QMatrix4x4 m_viewMatrix;
    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions = nullptr);
    void rotate(const QQuaternion &r);
    void translate(const QVector3D &t);
    void scale(const float &s);
    void setGlobalTransform(const QMatrix4x4 &g);

    void addObject(Transformational *obj);
};

#endif // CAMERA3D_H
