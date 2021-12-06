#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <CL/cl.hpp>

#include "data.h"

// Methods.
long get_all_platforms_opencl(std::vector<cl::Platform>& all_platforms);
long get_all_devices_opencl(std::vector<cl::Device>& all_devices, cl::Platform default_platform);
cl_ulong find_bucket_position_opencl(cl_ulong* bucket_frequency, cl_double percentile, cl_ulong numbers_count, cl_ulong numbers_count_under_min);
long get_bucket_opencl(std::ifstream& stream, HISTOGRAM& histogram, Histogram_Object& result_bucket, double percentile, int64_t min, int64_t max, std::string input_device);

// Kernel functions.
const std::string kernel_src = R"===(

#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_int64_extended_atomics : enable

constant ulong mask = (((ulong)1) << 63);

/// <summary>
/// Converts unsigned integer value with sign magnitude to signed integer value.
/// </summary>
/// <param name="value">Sign magnitude unsigned integer.</param>
/// <returns>Signed integer value.</returns>
long convert_from_sign_magnitude(ulong value)
{	
	// Memcpy not allowed, we need to convert it using union.
	union { long l; ulong u; } united_values = { .u = value };

    if (united_values.l >= 0)
    {
        return united_values.l;
    }
    else
    {
        // If integer is lower than zero, then we need to negate first bit and return that value with minus.
        long converted = (long)(value & (~mask));

        return -converted;
    }
}

/// <summary>
/// Process data value and increment frequency of specific bucket.
/// </summary>
/// <param name="bucket_frequency">Array of bucket frequencies.</param>
/// <param name="numbers_count">Numbers count.</param>
/// <param name="numbers_count_under_min">Numbers count under minimum.</param>
/// <param name="data_buffer">Data buffer with double values.</param>
/// <param name="min">Minimal value.</param>
/// <param name="max">Maximal value.</param>
/// <param name="bucket_size">Size of bucket.</param>
/// <param name="bucket_index_offset">Offset of bucket index.</param>
__kernel void process_data_value(global ulong* bucket_frequency, global ulong* numbers_count, global ulong* numbers_count_under_min,
								 global double* data_buffer, long min, long max, long bucket_size, long bucket_index_offset)
{
	// Get double value from vector based on process id.
	size_t in_index = get_global_id(0);
	double value = data_buffer[in_index];

	// Check if double value is correct value.
    if (!isnormal(value) && value != -0.0 && value != 0.0)
	{
        return;
	}

	// Memcpy not allowed, we need to convert it using union.
	union { double d; ulong u; } united_values = { .d = value };
	long i_value = convert_from_sign_magnitude(united_values.u);	

	// Check if value is in histogram range.
	if (i_value <= max && i_value >= min)
	{				
		ulong index = (i_value / bucket_size) + bucket_index_offset;
		atom_inc(&bucket_frequency[index]);
	}
	else if (i_value < min)
	{
		// Increment counter for values under minimum.
		atom_inc(numbers_count_under_min);
	}

	// Increment total counts.
	atom_inc(numbers_count);
	
	return;
}

)===";