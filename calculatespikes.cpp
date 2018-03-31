#include "calculatespikes.h"

QVector<QVector3D> spikeCalculation(Identify &detail)
{
    QVector<QVector3D> detailSpikes;
    if (detail.id == Base24x24)
        spikeCalculationZxX(detail.sp_length, detail.sp_width, 0, detailSpikes);
    else if (detail.id == Cube2x3 || detail.id == Plate2x3 || detail.id == Angle2x3 || detail.id == Anglev2x3)
        spikeCalculation2x3(detail.sp_height, detailSpikes);
    else if ((detail.id >= Cube2x2 && detail.id <= Cube8x8) || detail.id == Angle2x2 || detail.id == Anglev2x2)
        spikeCalculationZxX(detail.sp_length, detail.sp_width, standart_height, detailSpikes);
    else if (detail.id >= Plate2x2 && detail.id <= Plate8x8)
        spikeCalculationZxX(detail.sp_length, detail.sp_width, plate_height, detailSpikes);
    else if ((detail.id >= Cube1x1 && detail.id <= Cube1x10) || (detail.id >= Plate1x1 && detail.id <= Plate1x10)
             || (detail.id >= Arc1x3 && detail.id <= Arc1x6) || (detail.id >= Angle1x2 && detail.id <= Angle1x3))
    {
        if (detail.sp_length % 2 == 0)
            spikeCalculation1xEven(detail.sp_length, detail.sp_height, detailSpikes);
        else
            spikeCalculation1xOdd (detail.sp_length, detail.sp_height, detailSpikes);
    }

    return detailSpikes;
}

void spikeCalculation1xOdd(const int sp_length, const int sp_height, QVector<QVector3D> &detailSpikes)
{
    float det_height = standart_height;
    if (sp_height == 1)
        det_height = plate_height;
    detailSpikes.append(QVector3D(half_one_width, det_height + spike_height, half_one_width));
    for (int i = 0; i < (sp_length - 1)/2; i += 2)
    {        
        detailSpikes.append(QVector3D(detailSpikes[i].x() - between_spikes_width, detailSpikes[i].y(), detailSpikes[i].z()));
        detailSpikes.append(QVector3D(detailSpikes[i].x() + between_spikes_width, detailSpikes[i].y(), detailSpikes[i].z()));
    }
}

void spikeCalculation1xEven(const int sp_length, const int sp_height, QVector<QVector3D> &detailSpikes)
{
    float det_height = standart_height;
    if (sp_height == 1)
        det_height = plate_height;
    QVector3D l, r;
    l.setX(-half_one_width);
    l.setY(det_height + spike_height);
    l.setZ(half_one_width);
    r.setX(half_one_width);
    r.setY(det_height + spike_height);
    r.setZ(half_one_width);
    for (int i = 0; i < sp_length/2; i++)
    {
        detailSpikes.append(l);
        detailSpikes.append(r);
        l.setX(l.x() - between_spikes_width);
        r.setX(r.x() + between_spikes_width);
    }
}

void spikeCalculationZxX(const int sp_length, const int sp_width, const float height, QVector<QVector3D> &detailSpikes)
{
    QVector3D a;
    a.setX(half_one_width - sp_length*half_one_width);
    a.setY(height + spike_height);
    a.setZ(half_one_width - sp_width*half_one_width);  // Сдвиг в дальний левый край.

    for (int i = 0; i < sp_width; i++)
    {
        float tmpX = a.x();
        for (int j = 0; j < sp_length; j++)
        {
            detailSpikes.append(a);
            a.setX(a.x() + between_spikes_width);
        }
        a.setX(tmpX);
        a.setZ(a.z() + between_spikes_width);
    }
}

void spikeCalculation2x3(const int sp_height, QVector<QVector3D> &detailSpikes)
{
    float det_height = standart_height;
    if (sp_height == 1)
        det_height = plate_height;
    detailSpikes.append(QVector3D(half_one_width, det_height + spike_height, half_one_width));
    detailSpikes.append(QVector3D(detailSpikes[0].x() - between_spikes_width, detailSpikes[0].y(), detailSpikes[0].z()));
    detailSpikes.append(QVector3D(detailSpikes[0].x() + between_spikes_width, detailSpikes[0].y(), detailSpikes[0].z()));
    for (int i = 0; i < 3; i++)
        detailSpikes.append(QVector3D(detailSpikes[i].x(), detailSpikes[i].y(), detailSpikes[i].z() - between_spikes_width));
}
