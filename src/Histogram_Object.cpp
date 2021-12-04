#include "Histogram_Object.h"

/// <summary>
/// Constructor.
/// </summary>
Histogram_Object::Histogram_Object(uint64_t index)
{
	this->frequency = 0;
	this->index = index;
}

/// <summary>
/// Increment frequency.
/// </summary>
void Histogram_Object::increment_frequency()
{
	this->frequency++;
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
/// Add frequency to current bucket frequency (using when merging histograms).
/// </summary>
/// <param name="frequency">Frequency.</param>
void Histogram_Object::add_frequency(size_t frequency)
{
	this->frequency += frequency;
}

/// <summary>
/// Reset all values for bucket.
/// </summary>
void Histogram_Object::reset_values()
{
	this->frequency = 0;
}

/// <summary>
/// Get bucket index.
/// </summary>
/// <returns></returns>
uint64_t Histogram_Object::get_index()
{
	return this->index;
}
