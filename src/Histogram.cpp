#include "Histogram.h"

Histogram::Histogram()
{
	this->bucketed = false;
}

bool Histogram::isBucketed()
{
	return this->bucketed;
}

void Histogram::setBucketed(bool pBucketed)
{
	this->bucketed = pBucketed;
}

std::list<HistogramObject> Histogram::getObjects()
{
	return this->objects;
}