#pragma once
class HistogramObject
{
	private: 
		double min;
		double max;
		long frequency;
		double percentile;

	public:
		HistogramObject();
		HistogramObject(double pMin, double pMax);
		void incrementFrequency();
		double getMin();
		double getMax();
		bool isBucketed();
		long getFrequency();
		double calculatePercentile(long cumulativeFrequency, long numbersCount);
		double getPercentile();
};

