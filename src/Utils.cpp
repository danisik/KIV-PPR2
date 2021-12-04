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

uint64_t Utils::convert_int_to_uint(int64_t value)
{
    uint64_t u_value;
    std::memcpy(&u_value, &value, sizeof(value));

    return u_value;
}

int64_t Utils::convert_uint_to_int(uint64_t value)
{
    int64_t i_value;
    std::memcpy(&i_value, &value, sizeof(value));

    return i_value;
}

int64_t Utils::convert_from_sign_magnitude(uint64_t value)
{
    int64_t i_value;
    std::memcpy(&i_value, &value, sizeof(i_value));

    if (i_value >= 0)
    {
        return i_value;
    }
    else
    {
        int64_t converted = (int64_t)(value & (~mask));

        return -converted;
    }
}

uint64_t Utils::convert_to_sign_magnitude(int64_t value)
{
    uint64_t u_value;

    if (value >= 0)
    {        
        std::memcpy(&u_value, &value, sizeof(u_value));

        return u_value;
    }
    else
    {
        u_value = (uint64_t)(-value);

        return u_value | mask;
    }
}

uint64_t Utils::get_value_from_index(size_t index, int64_t bucket_size, int64_t index_offset)
{
    int64_t value = (index - index_offset) * bucket_size;

    return convert_to_sign_magnitude(value);
}

uint64_t Utils::get_index_from_value(int64_t value, int64_t bucket_size, int64_t index_offset)
{
    uint64_t index = (value / bucket_size) + index_offset;

    return index;
}