#ifndef GROUP3D_H
#define GROUP3D_H

#include <QVector>
#include <QQuaternion>
#include <QMatrix4x4>
#include "transformational.h"

class Group3D : public Transformational
{   
private:
    QQuaternion m_rotate;
    QVector3D m_translate;
    float m_scale;
    QMatrix4x4 m_globalTransform;
    QVector<Transformational *> m_objects; // * для того, чтобы группа могла содержать не только объекты, но и другие группы.
    void fillLocalMatrix(QMatrix4x4 &lm);

public:
    Group3D();
    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions);
    void rotate(const QQuaternion &r);
    void translate(const QVector3D &t);
    void scale(const float &s);
    void setGlobalTransform(const QMatrix4x4 &g);

    void addObject(Transformational *obj);
    void delObject(Transformational *obj);
    void delObject(const int &index);
};

#endif // GROUP3D_H
