#include <limits>

#pragma once
/// <summary>
/// Class representing single bucket in histogram.
/// </summary>
class Histogram_Object
{
	private: 
		// Bucket minimum value.
		double min;
		// Bucket maximum value.
		double max;
		// Found minimum value in file.
		double min_value_file;
		// Found maximum value in file.
		double max_value_file;
		// Number count in bucket.
		size_t frequency;

	public:
		Histogram_Object();
		Histogram_Object(double pMin, double pMax);
		void increment_frequency();
		double get_min();
		double get_max();
		void set_min(double min);
		void set_max(double max);
		size_t get_frequency();
		double get_min_value_file();
		double get_max_value_file();
		void set_min_value_file(double pMin);
		void set_max_value_file(double pMax);
		void reset_values();
		void add_frequency(size_t frequency);
};

