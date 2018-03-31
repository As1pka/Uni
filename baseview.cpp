#include "baseview.h"
#include "simpleobject3d.h"
#include "group3d.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QString>
#include <QTextStream>
#include <limits.h>

#include "camera3d.h"

BaseView::BaseView(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_camera = new Camera3D;
    m_camera->translate(QVector3D(0., 0., -250.));
    isPlant = 0;
    rotate_koef = 1;
}

BaseView::~BaseView()
{
    delete m_camera;
    for (int i = 0; i < m_object.size(); i++)
        delete m_object[i];

    for (int i = 0; i < m_groups.size(); i++)
        delete m_groups[i];

    for (int i = 0; i < m_transformObjects.size(); i++)
        delete m_transformObjects[i];
}

void BaseView::initializeGL()
{
    glClearColor(0., 0., 0., 1.);

    glEnable(GL_DEPTH_TEST);    // Глубина.
    glEnable(GL_CULL_FACE);     // Отсечение.

    initShaders();

    loadObj(":/Base24x24.obj", QColor(255, 0, 0, 255), Identify(Base24x24, 24, 24, 0));
    m_object.last()->translate(QVector3D(0., -1.5, 0.));
    m_transformObjects.append(m_object.last());
}

void BaseView::resizeGL(int w, int h)
{
    float aspect = w / (h ? (float)h : 1);

    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(45, aspect, 0.1f, 10000.);
    m_orthoMatrix.setToIdentity();
    m_orthoMatrix.ortho(0, 0, w_width, w_height, 0.1f, 1000.);
    m_orthoMatrix = m_orthoMatrix.inverted();
}

void BaseView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program.bind();
    // Передача параметров в шейдер.
    m_program.setUniformValue("u_projectionMatrix", m_projectionMatrix);
    m_program.setUniformValue("u_lightPosition", QVector4D(0., 0., 0., 0.1));
    m_program.setUniformValue("u_lightPower", 1.f);

    m_camera->draw(&m_program);
    for (int i = 0; i < m_transformObjects.size(); i++)
        m_transformObjects[i]->draw(&m_program, context()->functions());
}

void BaseView::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() == Qt::LeftButton)
    {
        m_mouseLeftPosition  = QVector2D(e->localPos());
        this->setFocus();
    }
    if (e->buttons() == Qt::RightButton)
        m_mouseRightPosition = QVector2D(e->localPos());
    e->accept();
}

void BaseView::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() == Qt::LeftButton)
    {
        QVector2D diff = QVector2D(e->localPos()) - m_mouseLeftPosition;
        m_mouseLeftPosition = QVector2D(e->localPos());

        float angle = diff.length() / 2.; // Коэффициент не статичен.
        QVector3D axis = QVector3D(diff.y(), diff.x(), 0.); // Вектор перпендикулярно движению мыши.
        m_camera->rotate(QQuaternion::fromAxisAndAngle(axis,angle));
    }
    else if (e->buttons() == Qt::RightButton)
    {
        QVector2D diff = QVector2D(e->localPos()) - m_mouseRightPosition;
        m_mouseRightPosition = QVector2D(e->localPos());

        m_camera->translate(QVector3D (diff.x()/2., -diff.y()/2., 0));
    }
    else
        return;

    update();
}

void BaseView::wheelEvent(QWheelEvent *e)
{
    if (e->delta() > 0)
        m_camera->translate(QVector3D(0., 0.,  5.));
    else if (e->delta() < 0 )
        m_camera->translate(QVector3D(0., 0., -5.));
    update();
}

void BaseView::keyPressEvent(QKeyEvent *e)
{
    if (isPlant)
    {
        switch (e->key())
        {
        case Qt::Key_Left:
            m_object.last()->translateWSP(QVector3D(-between_spikes_width, 0, 0));
            correctMove();
            break;
        case Qt::Key_Right:
            m_object.last()->translateWSP(QVector3D(between_spikes_width, 0, 0));
            correctMove();
            break;
        case Qt::Key_Down:
            m_object.last()->translateWSP(QVector3D(0, 0, between_spikes_width));
            correctMove();
            break;
        case Qt::Key_Up:
            m_object.last()->translateWSP(QVector3D(0, 0, -between_spikes_width));
            correctMove();
            break;
        case Qt::Key_R:
            if (isPlant == 2) // 1x1
                break;
            m_object.last()->rotateWSP(QQuaternion::fromAxisAndAngle(0, 1, 0, 90));

            if (isPlant != 3)
            {
                m_object.last()->translateWSP(QVector3D(rotate_koef*half_one_width, 0, rotate_koef*half_one_width));
                rotate_koef *= -1;
            }
            correctMove();
            break;
        case Qt::Key_Return:    // Qt::Key_Return - стандартный энтер.
            correctActive();
            isPlant = 0;
            rotate_koef = 1;//0
            return;
        case Qt::Key_Enter:     // Qt::Key_Enter - нампадовский энтер.
            correctActive();
            isPlant = 0;
            rotate_koef = 1;//0
            return;
        case Qt::Key_Escape:
            m_object.removeLast();
            m_transformObjects.removeLast();
            isPlant = 0;
            rotate_koef = 1;//0
            break;
        }
        update();
    }
}

void BaseView::correctMove()
{
    QVector<float> y;
    for (int i = 0; i < 6; i++)
        y.append(spike_height); // Невозможная величина для данной переменной.

    for (int i = 0; i < m_object.last()->pipes.size(); i++)
    {
        float currXRound = roundF10(m_object.last()->pipes[i].x());
        float currYRound = roundF10(m_object.last()->pipes[i].y());
        float currZRound = roundF10(m_object.last()->pipes[i].z());
        for (int j = 0; j < active_spikes.size(); j++)
            for (int k = 0; k < active_spikes[j].size(); k++)
            {
                float activeXRound = roundF10(active_spikes[j][k].x());
                float activeZRound = roundF10(active_spikes[j][k].z());
                if (currXRound == activeXRound && currZRound == activeZRound)
                {
                    float currYPlusStRound = roundF10(m_object.last()->pipes[i].y() + standart_height);
                    float currYPlusPlRound = roundF10(m_object.last()->pipes[i].y() + plate_height);
                    float activeYRound = roundF10(active_spikes[j][k].y());
                     if (currYPlusStRound == activeYRound)
                        y[0] = standart_height; // Подъем.
                    else if (currYPlusPlRound == activeYRound)
                        y[1] = plate_height; // Подъем.
                    else if (currYRound == activeYRound)
                        y[2] = 0;               // На месте.
                    else if (currYPlusStRound > activeYRound && currYRound < activeYRound)
                    {
                        float tmp = active_spikes[j][k].y() - m_object.last()->pipes[i].y();
                        if (tmp > y[3] || y[3] == spike_height)
                            y[3] = tmp;         // Подъем.
                    }
                    else if (currYPlusStRound > activeYRound)
                    {
                        float tmp = -(m_object.last()->pipes[i].y() - active_spikes[j][k].y());
                        if (tmp > y[4] || y[4] == spike_height)
                            y[4] = tmp;         // Спуск.
                    }
                    else if (currYPlusStRound < activeYRound)
                    {
                        float tmp = active_spikes[j][k].y() - m_object.last()->pipes[i].y();
                        if (tmp > y[5] || y[5] == spike_height)
                            y[5] = tmp;         // Подъем.
                    }
                }
            }
    }

    float max = -INT_MAX;
    for (int i = 0; i < y.size(); i++)
    {
        if (y[i] == spike_height)
        {
            y.remove(i);
            i--;
        }
        else if (max < y[i])
            max = y[i];
    }

    if (y.size() != 0)
        m_object.last()->translateWSP(QVector3D(0, max, 0));
}

void BaseView::correctActive()
{
    for (int i = 0; i < m_object.last()->spikes.size(); i++)        
    {
        float currXRound = roundF10(m_object.last()->spikes[i].x());
        float currYRound = roundF10(m_object.last()->spikes[i].y());
        float currZRound = roundF10(m_object.last()->spikes[i].z());
        for (int j = 0; j < active_spikes.size(); j++)
            for (int k = 0; k < active_spikes[j].size(); k++)
            {
                float activeXRound = roundF10(active_spikes[j][k].x());
                float activeYRound = roundF10(active_spikes[j][k].y());
                float activeZRound = roundF10(active_spikes[j][k].z());
                if (currXRound == activeXRound && currZRound == activeZRound)
                    if (currYRound > activeYRound) // тут бы еще проверку, что активный шип встает в трубку. если да то обновляем активный шип, иначе оставляем активный шип, добавляем новые шипы и добавляем новые активные трубки (единственный способ добавления активных трубок) + проверка на активные трубки
                        active_spikes[j][k].setY(m_object.last()->spikes[i].y());
            }
    }
}

void BaseView::initShaders()
{
    if (!m_program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,   ":/vshader.vsh"))
        close();
    if (!m_program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.fsh"))
        close();
    if (!m_program.link()) // Склейка шейдеров.
        close();
}

void BaseView::loadObj(const QString &path, QColor dColor, Identify detail)
{
    QFile objFile(path);
    if (!objFile.exists())
    {
        qDebug() << "File not found";
        return;
    }
    QVector<QVector3D> coords;
    QVector<QVector2D> texCoords;
    QVector<QVector3D> normals;

    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;

    objFile.open(QIODevice::ReadOnly);
    QTextStream input(&objFile);
    while (!input.atEnd()){
        QString str = input.readLine();
        QStringList list = str.split(" ");
        if (list[0] == "#")
        {
            qDebug() << "This is comment: " << str;
            continue;
        }
        else if (list[0] == "mtllib")
        {
            qDebug() << "File with materials: " << str;
        }
        else if (list[0] == "v")
        {
            coords.append(QVector3D(list[1].toFloat(), list[2].toFloat(), list[3].toFloat()));
            continue;
        }
        else if (list[0] == "vt")
        {
            texCoords.append(QVector2D(list[1].toFloat(), list[2].toFloat()));
            continue;
        }
        else if (list[0] == "vn")
        {
            normals.append(QVector3D(list[1].toFloat(), list[2].toFloat(), list[3].toFloat()));
            continue;
        }
        else if (list[0] == "f")
        {
            for (int i = 1; i <= 3; i++)
            {
                QStringList vert = list[i].split("/");
                vertexes.append(VertexData(coords[vert[0].toLong() - 1], (texCoords.size()?texCoords[vert[1].toLong() - 1]:QVector2D(0.,0.)), normals[vert[2].toLong()-1]));
                indexes.append(indexes.size());
            }
            continue;
        }
    }
    QImage img(256, 256, QImage::Format_ARGB32);
    img.fill(dColor);
    QVector<QVector3D> d_spikes = spikeCalculation(detail);
    if (detail.id == Base24x24)
    {
        active_spikes.append(d_spikes);
        m_object.append(new SimpleObject3D(vertexes, indexes, img, d_spikes));
    }
    else
    {
        QVector<QVector3D> d_pipes = pipeCalculation(detail, d_spikes);
        m_object.append(new SimpleObject3D(vertexes, indexes, img, d_spikes, d_pipes));
    }

    objFile.close();
}

void BaseView::getDetail(int idDetail, QColor dColor)
{
    isPlant++;
    switch (idDetail)
    {
    case Cube1x1:
        loadObj(":/Cube1x1.obj",  dColor, Identify(idDetail, 1, 1));
        m_object.last()->translate(QVector3D(half_one_width, 0., half_one_width)); // Сдвиг детали относительно центра на пол детали вправо и к нам.
        correctMove();
        isPlant++; // Не выполняется поворот.
        qDebug() << "good";
        break;
    case Cube1x2:
        loadObj(":/Cube1x2.obj",  dColor, Identify(idDetail, 1, 2));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Cube1x3:
        loadObj(":/Cube1x3.obj",  dColor, Identify(idDetail, 1, 3));
        m_object.last()->translate(QVector3D(half_one_width, 0., half_one_width)); // Сдвиг детали относительно центра на пол детали вправо и к нам.
        correctMove();
        isPlant += 2; // После поворота не выполняется сдвиг.
        qDebug() << "good";
        break;
    case Cube1x4:
        loadObj(":/Cube1x4.obj",  dColor, Identify(idDetail, 1, 4));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Cube1x6:
        loadObj(":/Cube1x6.obj",  dColor, Identify(idDetail, 1, 6));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Cube1x8:
        loadObj(":/Cube1x8.obj",  dColor, Identify(idDetail, 1, 8));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Cube1x10:
        loadObj(":/Cube1x10.obj", dColor, Identify(idDetail, 1, 10));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Cube2x2:
        loadObj(":/Cube2x2.obj", dColor, Identify(idDetail, 2, 2));
        correctMove();
        isPlant++;
        qDebug() << "good";
        break;
    case Cube2x3:
        loadObj(":/Cube2x3.obj", dColor, Identify(idDetail, 2, 3));
        m_object.last()->translate(QVector3D(half_one_width, 0., 0.));
        correctMove();
        qDebug() << "good";
        break;
    case Cube2x4:
        loadObj(":/Cube2x4.obj", dColor, Identify(idDetail, 2, 4));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;
    case Cube2x6:
        loadObj(":/Cube2x6.obj", dColor, Identify(idDetail, 2, 6));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;
    case Cube2x8:
        loadObj(":/Cube2x8.obj", dColor, Identify(idDetail, 2, 8));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;
    case Cube2x10:
        loadObj(":/Cube2x10.obj", dColor, Identify(idDetail, 2, 10));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;

    case Cube4x4:
        loadObj(":/Cube4x4.obj", dColor, Identify(idDetail, 4, 4));
        isPlant++;
        correctMove();
        qDebug() << "good";
        break;
    case Cube4x6:
        loadObj(":/Cube4x6.obj", dColor, Identify(idDetail, 4, 6));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;
    case Cube4x8:
        loadObj(":/Cube4x8.obj", dColor, Identify(idDetail, 4, 8));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;
    case Cube4x10:
        loadObj(":/Cube4x10.obj", dColor, Identify(idDetail, 4, 10));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;

    case Cube8x8:
        loadObj(":/Cube8x8.obj", dColor, Identify(idDetail, 8, 8));
        isPlant++;
        correctMove();
        qDebug() << "good";
        break;

    case Plate1x1:
        loadObj(":/Plate1x1.obj",  dColor, Identify(idDetail, 1, 1, 1));
        m_object.last()->translate(QVector3D(half_one_width, 0., half_one_width)); // Сдвиг детали относительно центра на пол детали вправо и к нам.
        correctMove();
        isPlant++;
        qDebug() << "good";
        break;
    case Plate1x2:
        loadObj(":/Plate1x2.obj",  dColor, Identify(idDetail, 1, 2, 1));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Plate1x3:
        loadObj(":/Plate1x3.obj",  dColor, Identify(idDetail, 1, 3, 1));
        m_object.last()->translate(QVector3D(half_one_width, 0., half_one_width)); // Сдвиг детали относительно центра на пол детали вправо и к нам.
        correctMove();
        isPlant += 2;
        qDebug() << "good";
        break;
    case Plate1x4:
        loadObj(":/Plate1x4.obj",  dColor, Identify(idDetail, 1, 4, 1));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Plate1x6:
        loadObj(":/Plate1x6.obj",  dColor, Identify(idDetail, 1, 6, 1));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Plate1x8:
        loadObj(":/Plate1x8.obj",  dColor, Identify(idDetail, 1, 8, 1));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Plate1x10:
        loadObj(":/Plate1x10.obj", dColor, Identify(idDetail, 1, 10, 1));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;

    case Plate2x2:
        loadObj(":/Plate2x2.obj", dColor, Identify(idDetail, 2, 2, 1));
        correctMove();
        isPlant++;
        qDebug() << "good";
        break;
    case Plate2x3:
        loadObj(":/Plate2x3.obj", dColor, Identify(idDetail, 2, 3, 1));
        m_object.last()->translate(QVector3D(half_one_width, 0., 0.));
        correctMove();
        qDebug() << "good";
        break;
    case Plate2x4:
        loadObj(":/Plate2x4.obj", dColor, Identify(idDetail, 2, 4, 1));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;
    case Plate2x6:
        loadObj(":/Plate2x6.obj", dColor, Identify(idDetail, 2, 6, 1));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;
    case Plate2x8:
        loadObj(":/Plate2x8.obj", dColor, Identify(idDetail, 2, 8, 1));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;
    case Plate2x10:
        loadObj(":/Plate2x10.obj", dColor, Identify(idDetail, 2, 10, 1));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;

    case Plate4x4:
        loadObj(":/Plate4x4.obj", dColor, Identify(idDetail, 4, 4, 1));
        isPlant++;
        correctMove();
        qDebug() << "good";
        break;
    case Plate4x6:
        loadObj(":/Plate4x6.obj", dColor, Identify(idDetail, 4, 6, 1));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;
    case Plate4x8:
        loadObj(":/Plate4x8.obj", dColor, Identify(idDetail, 4, 8, 1));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;
    case Plate4x10:
        loadObj(":/Plate4x10.obj", dColor, Identify(idDetail, 4, 10, 1));
        isPlant += 2;
        correctMove();
        qDebug() << "good";
        break;

    case Plate8x8:
        loadObj(":/Plate8x8.obj", dColor, Identify(idDetail, 8, 8, 1));
        isPlant++;
        correctMove();
        qDebug() << "good";
        break;

    case Arc1x3:
        loadObj(":/Arc1x3.obj",  dColor, Identify(idDetail, 1, 3));
        m_object.last()->translate(QVector3D(half_one_width, 0., half_one_width)); // Сдвиг детали относительно центра на пол детали вправо и к нам.
        correctMove();
        isPlant += 2; // После поворота не выполняется сдвиг.
        qDebug() << "good";
        break;
    case Arc1x4:
        loadObj(":/Arc1x4.obj",  dColor, Identify(idDetail, 1, 4));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Arc1x6:
        loadObj(":/Arc1x6.obj",  dColor, Identify(idDetail, 1, 6));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;

    case Angle1x2:
        loadObj(":/Angle1x2.obj",  dColor, Identify(idDetail, 1, 2));
        m_object.last()->translate(QVector3D(0., 0., half_one_width)); // Сдвиг детали относительно центра на пол детали к нам.
        correctMove();
        qDebug() << "good";
        break;
    case Angle1x3:
        loadObj(":/Angle1x3.obj",  dColor, Identify(idDetail, 1, 3));
        m_object.last()->translate(QVector3D(half_one_width, 0., half_one_width)); // Сдвиг детали относительно центра на пол детали вправо и к нам.
        correctMove();
        isPlant += 2; // После поворота не выполняется сдвиг.
        qDebug() << "good";
        break;
    case Angle2x2:
        loadObj(":/Angle2x2.obj", dColor, Identify(idDetail, 2, 2));
        correctMove();
        isPlant += 2;
        qDebug() << "good";
        break;
    case Angle2x3:
        loadObj(":/Angle2x3.obj", dColor, Identify(idDetail, 2, 3));
        m_object.last()->translate(QVector3D(half_one_width, 0., 0.));
        correctMove();
        qDebug() << "good";
        break;

    case Anglev2x2:
        loadObj(":/Anglev2x2.obj", dColor, Identify(idDetail, 2, 2));
        correctMove();
        isPlant += 2;
        qDebug() << "good";
        break;
    case Anglev2x3:
        loadObj(":/Anglev2x3.obj", dColor, Identify(idDetail, 2, 3));
        m_object.last()->translate(QVector3D(half_one_width, 0., 0.));
        correctMove();
        qDebug() << "good";
        break;
    default:
        isPlant = 0;
        qDebug() << "default";
        return;
    }
    m_transformObjects.append(m_object.last());
    update();
}

void BaseView::getSize(int w, int h)
{
    w_width  = w; // X
    w_height = h; // Y
}
