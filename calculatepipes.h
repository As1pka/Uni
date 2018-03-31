#ifndef CALCULATEPIPES_H
#define CALCULATEPIPES_H

#include <QMatrix4x4>
#include <QtCore/qmath.h>
#include "types.h"

//#define without_edge_width 4.8f
//#define edge_width         1.5f не используется
//#define gap_width          0.2f
//#define half_one_width     3.9f // Промах на 0.1f
#define half_one_width       4.f
#define between_spikes_width 8.f
#define standart_height      9.6f
#define plate_height         3.2f
#define spike_height         1.7f
#define spike_dia            4.8f

QVector<QVector3D> pipeCalculation(Identify &detail, const QVector<QVector3D> &detailSpikes);
float roundF10(const float value);

#endif // CALCULATEPIPES_H
