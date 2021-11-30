#include "Utils.h"

/// <summary>
/// Check if value is correct.
/// </summary>
/// <param name="value">Value to be classified</param>
/// <returns>True if value is correct, false if not.</returns>
bool Utils::is_correct_value(double value)
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

/// <summary>
/// Binary search on histogram to find position of bucket by value (between min and max of bucket).
/// </summary>
/// <param name="buckets">Histogram</param>
/// <param name="left">Left index</param>
/// <param name="right">Right index</param>
/// <param name="value">Value to be found</param>
/// <returns>Position if value was found in any bucket, -1 if not.</returns>
size_t Utils::binary_search(std::vector<Histogram_Object>& buckets, size_t left, size_t right, double value)
{
    if (right >= left) 
    {       
        size_t mid = left + (right - left) / 2;
        double min = buckets[mid].get_min();
        double max = buckets[mid].get_max();

        // If the element is present at the middle itself
        if (value <= max && value >= min)
            return mid;

        // If element is smaller than mid, then it can only be present in left subarray
        if (min > value)
            return binary_search(buckets, left, mid - 1, value);

        // Else the element can only be present in right subarray
        return binary_search(buckets, mid + 1, right, value);
    }

    // We reach here when element is not present in array
    return -1;
}

/// <summary>
/// Perform string tolower on specific string.
/// </summary>
/// <param name="str">String to be lowered.</param>
/// <returns>Lowered string.</returns>
std::string Utils::to_lower(std::string& str)
{
    // Lower all characters in string.
    for (auto& c : str)
    {
        c = tolower(c);
    }

    return str;
}