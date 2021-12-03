#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <limits>
#include <cmath>
#include <CL/cl.hpp>

#include "Watchdog.h"

struct MIN_MAX_FILE_VALUES
{
	cl_double min_value_file;
	cl_double max_value_file;
};

cl_double test();

const cl_ulong BUCKET_COUNT_OPENCL = 1024;
const cl_ulong BLOCK_SIZE_OPENCL = 512;

const std::string kernel_src = R"===(

#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_int64_extended_atomics : enable

long binary_search(double* min_bucket, double* max_bucket, long left, long right, double value)
{
    while (right >= left)
    {
        long mid = left + ((right - left) >> 1);
        double min = min_bucket[mid];
        double max = max_bucket[mid];

        // If the element is present at the middle itself
        if (value <= max && value >= min)
		{
            return mid;
		}

        // If element is smaller than min, then it can only be present in left subarray
        if (min > value)
		{	
            right = mid - 1;
		}
		else
		{
			// Else the element can only be present in right subarray
			left = mid + 1;
		}
    }

    // We reach here when element is not present in array
    return -1;
}

__kernel void process_data_block(global double* min_bucket, global double* max_bucket, global ulong* bucket_frequency, global ulong* numbers_count, global ulong* numbers_count_under_min,
								 global double* value_file, global ulong* value_file_position, global double* data_buffer, double min, double max, ulong bucket_count)
{
	size_t in_index = get_global_id(0);
	double value = data_buffer[in_index];
	
	if (value <= max && value >= min)
	{
		int addition = 0;
		if (value == max)
		{
			addition = 1;
		}

		// Get position using binary search.
		long position = binary_search(min_bucket, max_bucket, 0, bucket_count - 1, value);

		// If position was found.
		if (position != -1)
		{
			if (position == (bucket_count - 1))
				addition = 0;
			
			atom_inc(&bucket_frequency[in_index]);
			value_file_position[in_index] = position + addition;
			value_file[in_index] = value;
		}
		else
		{
			atom_inc(&bucket_frequency[in_index]);
			value_file_position[in_index] = bucket_count - 1;
			value_file[in_index] = value;
		}
	}
	else if (value < min)
	{
		atom_inc(numbers_count_under_min);
	}
		
	atom_inc(numbers_count);
	
	return;
}

)===";