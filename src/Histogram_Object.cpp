#include "Histogram_Object.h"

/// <summary>
/// Constructor.
/// </summary>
/// <param name="pMin">Minimum value for bucket.</param>
/// <param name="pMax">Maximum value for bucket.</param>
Histogram_Object::Histogram_Object(double pMin, double pMax)
{
	this->min = pMin;
	this->max = pMax;
	this->frequency = 0;
	this->min_value_file = std::numeric_limits<double>::max();
	this->max_value_file = std::numeric_limits<double>::lowest();
}

/// <summary>
/// Constructor.
/// </summary>
Histogram_Object::Histogram_Object()
{
	this->min = 0;
	this->max = 0;
	this->frequency = 0;
	this->min_value_file = std::numeric_limits<double>::max();
	this->max_value_file = std::numeric_limits<double>::lowest();
}

/// <summary>
/// Increment frequency.
/// </summary>
void Histogram_Object::increment_frequency()
{
	this->frequency++;
}

/// <summary>
/// Get minimum allowed value for bucket.
/// </summary>
/// <returns>Minimum value.</returns>
double Histogram_Object::get_min()
{
	return this->min;
}

/// <summary>
/// Get maximum allowed value for bucket.
/// </summary>
/// <returns>Maximum value</returns>
double Histogram_Object::get_max()
{
	return this->max;
}

/// <summary>
/// Get number count for this bucket.
/// </summary>
/// <returns>Number count</returns>
size_t Histogram_Object::get_frequency()
{
	return this->frequency;
}

/// <summary>
/// Get minimum value found in file for this bucket.
/// </summary>
/// <returns>Minimum value found in file</returns>
double Histogram_Object::get_min_value_file()
{
	return this->min_value_file;
}

/// <summary>
/// Get maximum value found in file for this bucket.
/// </summary>
/// <returns>Maximum value found in file</returns>
double Histogram_Object::get_max_value_file()
{
	return this->max_value_file;
}

/// <summary>
/// Try to set minimum value in file for this bucket.
/// </summary>
/// <param name="pMin">Currently found value in file.</param>
void Histogram_Object::set_min_value_file(double pMin)
{
	if (this->min_value_file > pMin)
	{
		this->min_value_file = pMin;
	}
}

/// <summary>
/// Try to set maximum value in file for this bucket.
/// </summary>
/// <param name="pMax">Currently found value in file.</param>
void Histogram_Object::set_max_value_file(double pMax)
{
	if (this->max_value_file < pMax)
	{
		this->max_value_file = pMax;
	}
}

/// <summary>
/// Add frequency to current bucket frequency (using when merging histograms).
/// </summary>
/// <param name="frequency">Frequency.</param>
void Histogram_Object::add_frequency(size_t frequency)
{
	this->frequency += frequency;
}

void Histogram_Object::set_min(double min)
{
	this->min = min;
}

void Histogram_Object::set_max(double max)
{
	this->max = max;
}

void Histogram_Object::reset_values()
{
	this->min_value_file = std::numeric_limits<double>::max();
	this->max_value_file = std::numeric_limits<double>::lowest();
	this->frequency = 0;
}
