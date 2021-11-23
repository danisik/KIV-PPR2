#include "Utils.h"

bool Utils::isCorrectValue(double value)
{
	switch (fpclassify(value))
	{
	case FP_NORMAL:
	case FP_ZERO:
		return true;

	case FP_INFINITE:
	case FP_NAN:
	case FP_SUBNORMAL:
	default:
		return false;
	}
}

long Utils::binarySearch(std::vector<HistogramObject>& buckets, long left, long right, double value)
{
    if (right >= left) 
    {
        long mid = left + (right - left) / 2;

        // If the element is present at the middle
        // itself
        if (value < buckets[mid].getMax() && value >= buckets[mid].getMin())
            return mid;

        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (buckets[mid].getMin() > value)
            return binarySearch(buckets, left, mid - 1, value);

        // Else the element can only be present
        // in right subarray
        return binarySearch(buckets, mid + 1, right, value);
    }

    // We reach here when element is not
    // present in array
    return -1;
}