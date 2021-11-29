#include <limits>

#pragma once
/// <summary>
/// Class representing single bucket in histogram.
/// </summary>
class HistogramObject
{
	private: 
		// Bucket minimum value.
		double min;
		// Bucket maximum value.
		double max;
		// Found minimum value in file.
		double minValueFile;
		// Found maximum value in file.
		double maxValueFile;
		// Number count in bucket.
		size_t frequency;

	public:
		HistogramObject();
		HistogramObject(double pMin, double pMax);
		void incrementFrequency();
		double getMin();
		double getMax();
		size_t getFrequency();
		double getMinValueFile();
		double getMaxValueFile();
		void setMinValueFile(double pMin);
		void setMaxValueFile(double pMax);
		void addFrequency(size_t frequency);
};

