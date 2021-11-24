#pragma once
class HistogramObject
{
	private: 
		double min;
		double max;
		long frequency;

	public:
		HistogramObject();
		HistogramObject(double pMin, double pMax);
		void incrementFrequency();
		double getMin();
		double getMax();
		long getFrequency();
};

