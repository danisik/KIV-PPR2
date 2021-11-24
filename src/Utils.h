#include <math.h>
#include <float.h>
#include <iomanip>
#include <vector>

#include "HistogramObject.h"

#pragma once
class Utils
{
	public:
		static bool isCorrectValue(double value);
		static long binarySearch(std::vector<HistogramObject>& buckets, long l, long r, double value);
};

