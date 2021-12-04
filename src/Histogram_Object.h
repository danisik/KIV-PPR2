#include <limits>
#include <iomanip>
#include <math.h>
#include <float.h>

#pragma once
/// <summary>
/// Class representing single bucket in histogram.
/// </summary>
class Histogram_Object
{
	private: 
		// Number count in bucket.
		size_t frequency;

		// Bucket index. Used for calculating final values.
		uint64_t index;

	public:
		Histogram_Object(uint64_t index);
		void increment_frequency();
		size_t get_frequency();
		void reset_values();
		void add_frequency(size_t frequency);
		uint64_t get_index();
};

