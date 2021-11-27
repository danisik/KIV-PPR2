#include "HistogramObject.h"

HistogramObject::HistogramObject(double pMin, double pMax)
{
	this->min = pMin;
	this->max = pMax;
	this->frequency = 0;
	this->minValueFile = 0;
	this->maxValueFile = 0;
}

HistogramObject::HistogramObject()
{
	this->min = 0;
	this->max = 0;
	this->frequency = 0;
	this->minValueFile = 0;
	this->maxValueFile = 0;
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

size_t HistogramObject::getFrequency()
{
	return this->frequency;
}

double HistogramObject::getMinValueFile()
{
	return this->minValueFile;
}

double HistogramObject::getMaxValueFile()
{
	return this->maxValueFile;
}

void HistogramObject::setMinValueFile(double pMin)
{
	if (this->minValueFile == 0 || this->minValueFile > pMin)
	{
		this->minValueFile = pMin;
	}
}

void HistogramObject::setMaxValueFile(double pMax)
{
	if (this->maxValueFile == 0 || this->maxValueFile < pMax)
	{
		this->maxValueFile = pMax;
	}
}
