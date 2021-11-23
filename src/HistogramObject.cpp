#include "HistogramObject.h"

HistogramObject::HistogramObject(double pMin, double pMax)
{
	this->min = pMin;
	this->max = pMax;
	this->frequency = 0;
	this->percentile = 0;
}

HistogramObject::HistogramObject()
{
	this->min = 0;
	this->max = 0;
	this->frequency = 0;
	this->percentile = 0;
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

bool HistogramObject::isBucketed()
{
	if (this->min == this->max)
	{
		return false;
	}

	return true;
}

long HistogramObject::getFrequency()
{
	return this->frequency;
}

double HistogramObject::getPercentile()
{
	return this->percentile;
}
