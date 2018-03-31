#include "simpleobject3d.h"

#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

SimpleObject3D::SimpleObject3D()
    : m_indexBuffer(QOpenGLBuffer::IndexBuffer), m_texture(nullptr), m_scale(1.) { }

SimpleObject3D::SimpleObject3D(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes,
                               const QImage &texture, const QVector<QVector3D> &vec_spikes)
    : m_indexBuffer(QOpenGLBuffer::IndexBuffer), m_texture(nullptr), m_scale(1.)
{
    init(vertData, indexes, texture);
    spikes = vec_spikes;
}

SimpleObject3D::SimpleObject3D(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes,
                               const QImage &texture, const QVector<QVector3D> &vec_spikes,
                               const QVector<QVector3D> &vec_pipes)
    : m_indexBuffer(QOpenGLBuffer::IndexBuffer), m_texture(nullptr), m_scale(1.)
{
    init(vertData, indexes, texture);
    spikes = vec_spikes;
    pipes = vec_pipes;
}

SimpleObject3D::~SimpleObject3D()
{
    if (m_vertexBuffer.isCreated())
        m_vertexBuffer.destroy();
    if (m_indexBuffer.isCreated())
        m_indexBuffer.destroy();
    if (m_texture && m_texture->isCreated())
        m_texture->destroy();
}

void SimpleObject3D::init(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes, const QImage &texture)
{
    if (m_vertexBuffer.isCreated())
        m_vertexBuffer.destroy();
    if (m_indexBuffer.isCreated())
        m_indexBuffer.destroy();
    if (m_texture && m_texture->isCreated())
    {
        delete m_texture;
        m_texture = nullptr;
    }

    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(vertData.constData(), vertData.size() * sizeof(VertexData)); // Копируем данные в видеобуфер.
    m_vertexBuffer.release();

    m_indexBuffer.create();
    m_indexBuffer.bind();
    m_indexBuffer.allocate(indexes.constData(), indexes.size() * sizeof(GLuint));
    m_indexBuffer.release();

    m_texture = new QOpenGLTexture(texture.mirrored()); // Для корректного отображения.
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMinificationFilter(QOpenGLTexture::Linear);
    m_texture->setWrapMode(QOpenGLTexture::Repeat);
}

void SimpleObject3D::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    if (!m_vertexBuffer.isCreated() || !m_indexBuffer.isCreated())
        return;
    m_texture->bind(0);
    program->setUniformValue("u_texture", 0);

    QMatrix4x4 modelMatrix;
    fillModelMatrix(modelMatrix);

    program->setUniformValue("u_modelMatrix", modelMatrix);
    m_vertexBuffer.bind();
    m_indexBuffer.bind();
    int offset = 0;

    // Задаём атрибуты шейдера.
    int vertLock = program->attributeLocation("a_position");
    // Включаем использование данного атрибута.
    program->enableAttributeArray(vertLock);
    program->setAttributeBuffer(vertLock, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int textLock = program->attributeLocation("a_textcoord");
    program->enableAttributeArray(textLock);
    program->setAttributeBuffer(textLock, GL_FLOAT, offset, 2, sizeof(VertexData));

    offset += sizeof(QVector2D);

    int normLock = program->attributeLocation("a_normal");
    program->enableAttributeArray(normLock);
    program->setAttributeBuffer(normLock, GL_FLOAT, offset, 3, sizeof(VertexData));

    functions->glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);

    m_vertexBuffer.release();
    m_indexBuffer.release();
    m_texture->release();
}

void SimpleObject3D::rotate(const QQuaternion &r)
{
    m_rotate *= r;
}

void SimpleObject3D::translate(const QVector3D &t)
{
    m_translate += t;
}

void SimpleObject3D::scale(const float &s)
{
    m_scale *= s;
}

void SimpleObject3D::setGlobalTransform(const QMatrix4x4 &g)
{
    m_globalTransform = g;
}

void SimpleObject3D::rotateWSP(const QQuaternion &r)
{
    m_rotate *= r;
    QMatrix4x4 m;
    m.setToIdentity();
    m.translate(m_translate);
    m.rotate(r);
    m.translate(-m_translate);
    for (int i = 0; i < spikes.size(); i++)
        spikes.replace(i, m*spikes[i]);
    for (int i = 0; i < pipes.size(); i++)
        pipes.replace(i,  m*pipes[i]);
}

void SimpleObject3D::translateWSP(const QVector3D &t)
{
    m_translate += t;
    for (int i = 0; i < spikes.size(); i++)
        spikes[i] += t;
    for (int i = 0; i < pipes.size(); i++)
        pipes[i]  += t;
}

QMatrix4x4 SimpleObject3D::fillModelMatrix(QMatrix4x4 &mm)
{
    mm.setToIdentity();        // В реальности будет выполнены действия в следующем порядке.
    mm.translate(m_translate); // 3
    mm.rotate(m_rotate);       // 2
    mm.scale(m_scale);         // 1
    mm = m_globalTransform * mm;
    return mm;
}
