#include <math.h>
#include <float.h>
#include <iomanip>
#include <vector>

#include "HistogramObject.h"

static constexpr const unsigned int MB = 1024*1024;
static constexpr const double DOUBLE_MIN = std::numeric_limits<double>::lowest();
static constexpr const double DOUBLE_MAX = std::numeric_limits<double>::max();

#pragma once
class Utils
{
	public:
		static bool isCorrectValue(double value);
		static long binarySearch(std::vector<HistogramObject>& buckets, long l, long r, double value);
};

