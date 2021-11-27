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
		static size_t binarySearch(std::vector<HistogramObject>& buckets, size_t l, size_t r, double value);
};

