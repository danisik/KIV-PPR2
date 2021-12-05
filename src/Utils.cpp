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

/// <summary>
/// Converts signed integer value to unsigned integer values.
/// </summary>
/// <param name="value">Signed Integer value</param>
/// <returns>Unsigned integer value</returns>
uint64_t Utils::convert_int_to_uint(int64_t value)
{
    uint64_t u_value;
    std::memcpy(&u_value, &value, sizeof(value));

    return u_value;
}

/// <summary>
/// Converts unsigned integer value to signed value.
/// </summary>
/// <param name="value">Unsigned integer value</param>
/// <returns>Signed integer value</returns>
int64_t Utils::convert_uint_to_int(uint64_t value)
{
    int64_t i_value;
    std::memcpy(&i_value, &value, sizeof(value));

    return i_value;
}

/// <summary>
/// Converts unsigned integer value with sign magnitude to signed integer value.
/// </summary>
/// <param name="value">Sign magnitude unsigned integer.</param>
/// <returns>Signed integer value.</returns>
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
        // If integer is lower than zero, then we need to negate first bit and return that value with minus.
        int64_t converted = (int64_t)(value & (~mask));

        return -converted;
    }
}

/// <summary>
/// Converts signed integer value to sign magnitude unsigned integer.
/// </summary>
/// <param name="value">Signed integer value.</param>
/// <returns>Sign magnitude unsigned integer.</returns>
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

/// <summary>
/// Converts index to value.
/// </summary>
/// <param name="index">Bucket index</param>
/// <param name="bucket_size">Bucket size</param>
/// <param name="index_offset">Bucket index offset.</param>
/// <returns>Unsigned value in sign magnitude.</returns>
uint64_t Utils::get_value_from_index(size_t index, int64_t bucket_size, int64_t bucket_index_offset)
{
    int64_t value = (index - bucket_index_offset) * bucket_size;

    return convert_to_sign_magnitude(value);
}