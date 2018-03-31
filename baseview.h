#ifndef BASEVIEW_H
#define BASEVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include "calculatespikes.h"

class SimpleObject3D;
class Transformational;
class Group3D;
class Camera3D;

class BaseView : public QOpenGLWidget
{
    Q_OBJECT
private:
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_orthoMatrix;
    QOpenGLShaderProgram m_program;
    QVector2D m_mouseLeftPosition, m_mouseRightPosition; // Переменные для сохранения позиции при нажатии левой и правой кнопки мыши.
    //QVector2D m_mouseDetailPosition;                     // Позиция для перемещения деталей.
    QVector<SimpleObject3D *> m_object;
    QVector<Transformational *> m_transformObjects;
    QVector<Group3D *> m_groups;

    Camera3D *m_camera;
    int w_width, w_height;
    int rotate_koef;        // Счетчик для корректого поворота детали на клавишу R.

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);

    void correctMove();
    void correctActive();
    void initShaders();
    void loadObj(const QString &path, QColor dColor, Identify detail);

public:
    explicit BaseView(QWidget *parent = nullptr);
    ~BaseView();
    int isPlant;
    QVector<QVector<QVector3D>> active_spikes, active_pipes;

private slots:
    void getDetail(int idDetail, QColor dColor);
    void getSize(int w, int h);
};

#endif // BASEVIEW_H
