#ifndef CALCULATESPIKES_H
#define CALCULATESPIKES_H

#include "calculatepipes.h"

void spikeCalculation2x3(const int sp_height, QVector<QVector3D> &detailSpikes);
void spikeCalculationZxX(const int sp_length, const int sp_width,  const float height,  QVector<QVector3D> &detailSpikes);
void spikeCalculation1xEven(const int sp_length, const int sp_height, QVector<QVector3D> &detailSpikes);
void spikeCalculation1xOdd (const int sp_length, const int sp_height, QVector<QVector3D> &detailSpikes);
QVector<QVector3D> spikeCalculation (Identify &detail);

#endif // CALCULATESPIKES_H
