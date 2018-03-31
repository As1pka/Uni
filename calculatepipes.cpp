#include "calculatepipes.h"

QVector<QVector3D> pipeCalculation(Identify &detail, const QVector<QVector3D> &detailSpikes)
{
    float det_height = standart_height;
    if (detail.sp_height == 1)
        det_height = plate_height;
    QVector<QVector3D> detailPipes;
    /*
    if (detail.id >= Arc1x3 && detail.id <= Arc1x6) // Последний/предпоследний шипы - самые удаленные друг от друга шипы.
    {
        int pl = detailSpikes.size() - 2; // Предпоследний.
        for (int i = 0; i < pl; i++)
            detailPipes.append(QVector3D(detailSpikes[i].x(), -1, detailSpikes[i].z()));
        detailPipes.append(QVector3D(detailSpikes[pl].x(), detailSpikes[pl].y() - det_height, detailSpikes[pl].z()));
        detailPipes.append(QVector3D(detailSpikes.last().x(), detailSpikes.last().y() - det_height, detailSpikes.last().z()));
    }
    else*/
    for (int i = 0; i < detailSpikes.size(); i++)
        detailPipes.append(QVector3D(detailSpikes[i].x(), detailSpikes[i].y() - det_height, detailSpikes[i].z())); // Координата трубки будет совпадать с координатой активного шипа

    return detailPipes;
}

float roundF10(const float value)
{
    return qFloor(value*10 + .5)/10.;
}
