#include "HistogramObject.h"

HistogramObject::HistogramObject(double pMin, double pMax)
{
	this->min = pMin;
	this->max = pMax;
	this->frequency = 0;
}

HistogramObject::HistogramObject()
{
	this->min = 0;
	this->max = 0;
	this->frequency = 0;
}

void HistogramObject::incrementFrequency()
{
	this->frequency++;
}

double HistogramObject::getMin()
{
	return this->min;
}

double HistogramObject::getMax()
{
	return this->max;
}

long HistogramObject::getFrequency()
{
	return this->frequency;
}
