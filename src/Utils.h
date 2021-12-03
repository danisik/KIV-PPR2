#include <math.h>
#include <float.h>
#include <iomanip>
#include <vector>

#include "Histogram_Object.h"

#pragma once
/// <summary>
/// Class which contains usefull functions.
/// </summary>
class Utils
{
	public:
		static bool is_correct_value(double value);
		static long binary_search(std::vector<Histogram_Object>& buckets, long l, long r, double value);
		static std::string to_lower(std::string& str);
};

