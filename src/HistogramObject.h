#pragma once
class HistogramObject
{
	private: 
		double min;
		double max;
		double minValueFile;
		double maxValueFile;
		long frequency;

	public:
		HistogramObject();
		HistogramObject(double pMin, double pMax);
		void incrementFrequency();
		double getMin();
		double getMax();
		long getFrequency();
		double getMinValueFile();
		double getMaxValueFile();
		void setMinValueFile(double pMin);
		void setMaxValueFile(double pMax);
};

