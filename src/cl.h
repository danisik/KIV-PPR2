#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <limits>
#include <cmath>
#include <CL/cl.hpp>

#include "data.h"
#include "Watchdog.h"
#include "Utils.h"

struct MIN_MAX_FILE_VALUES
{
	cl_double min_value_file;
	cl_double max_value_file;
};

cl_int test(std::string input_platform);

const cl_ulong BUCKET_COUNT_OPENCL = 1024;
const cl_ulong BLOCK_SIZE_OPENCL = 512;

const std::string kernel_src = R"===(

#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_int64_extended_atomics : enable

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
		long position = 1;

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