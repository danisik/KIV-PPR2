#include <math.h>
#include <float.h>
#include <iomanip>
#include <vector>

#pragma once
/// <summary>
/// Class which contains usefull functions.
/// </summary>
class Utils
{
	public:
		static bool is_correct_value(double value);
		static std::string to_lower(std::string& str);
		static int64_t convert_uint_to_int(uint64_t value);
		static uint64_t convert_int_to_uint(int64_t value);
		static int64_t convert_from_sign_magnitude(uint64_t value);
		static uint64_t convert_to_sign_magnitude(int64_t value);
		static uint64_t get_value_from_index(size_t index, int64_t bucket_size, int64_t index_offset);
		static uint64_t get_index_from_value(int64_t value, int64_t bucket_size, int64_t index_offset);

	private:
		static const uint64_t mask = (((uint64_t)1) << 63);
};

