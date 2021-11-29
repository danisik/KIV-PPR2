#include "HistogramObject.h"

/// <summary>
/// Constructor.
/// </summary>
/// <param name="pMin">Minimum value for bucket.</param>
/// <param name="pMax">Maximum value for bucket.</param>
HistogramObject::HistogramObject(double pMin, double pMax)
{
	this->min = pMin;
	this->max = pMax;
	this->frequency = 0;
	this->minValueFile = std::numeric_limits<double>::max();
	this->maxValueFile = std::numeric_limits<double>::lowest();
}

/// <summary>
/// Constructor.
/// </summary>
HistogramObject::HistogramObject()
{
	this->min = 0;
	this->max = 0;
	this->frequency = 0;
	this->minValueFile = std::numeric_limits<double>::max();
	this->maxValueFile = std::numeric_limits<double>::lowest();
}

/// <summary>
/// Increment frequency.
/// </summary>
void HistogramObject::incrementFrequency()
{
	this->frequency++;
}

/// <summary>
/// Get minimum allowed value for bucket.
/// </summary>
/// <returns>Minimum value.</returns>
double HistogramObject::getMin()
{
	return this->min;
}

/// <summary>
/// Get maximum allowed value for bucket.
/// </summary>
/// <returns>Maximum value</returns>
double HistogramObject::getMax()
{
	return this->max;
}

/// <summary>
/// Get number count for this bucket.
/// </summary>
/// <returns>Number count</returns>
size_t HistogramObject::getFrequency()
{
	return this->frequency;
}

/// <summary>
/// Get minimum value found in file for this bucket.
/// </summary>
/// <returns>Minimum value found in file</returns>
double HistogramObject::getMinValueFile()
{
	return this->minValueFile;
}

/// <summary>
/// Get maximum value found in file for this bucket.
/// </summary>
/// <returns>Maximum value found in file</returns>
double HistogramObject::getMaxValueFile()
{
	return this->maxValueFile;
}

/// <summary>
/// Try to set minimum value in file for this bucket.
/// </summary>
/// <param name="pMin">Currently found value in file.</param>
void HistogramObject::setMinValueFile(double pMin)
{
	if (this->minValueFile == 0 || this->minValueFile > pMin)
	{
		this->minValueFile = pMin;
	}
}

/// <summary>
/// Try to set maximum value in file for this bucket.
/// </summary>
/// <param name="pMax">Currently found value in file.</param>
void HistogramObject::setMaxValueFile(double pMax)
{
	if (this->maxValueFile == 0 || this->maxValueFile < pMax)
	{
		this->maxValueFile = pMax;
	}
}

/// <summary>
/// Add frequency to current bucket frequency (using when merging histograms).
/// </summary>
/// <param name="frequency">Frequency.</param>
void HistogramObject::addFrequency(size_t frequency)
{
	this->frequency += frequency;
}
