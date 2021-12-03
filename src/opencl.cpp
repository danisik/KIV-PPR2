#include "cl.h"

#undef max
 
cl_long get_default_platform(cl::Platform& default_platform)
{
	//get all platforms (drivers)
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
	
    if(all_platforms.size() == 0)
	{
        std::cout << " No platforms found. Check OpenCL installation!\n";
        return 1;
    }
    default_platform = all_platforms[0];
	
	return 0;
}

cl_long get_all_devices(std::vector<cl::Device>& all_devices, cl::Platform default_platform)
{
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if(all_devices.size() == 0)
	{
        std::cout<<" No devices found. Check OpenCL installation!\n";
        return 1;
    }
	
	return 0;
}

cl_ulong find_bucket_opencl(cl_ulong* bucket_frequency, cl_double percentile, cl_ulong numbers_count, cl_ulong numbers_count_under_min, cl_ulong bucket_count)
{
	// Calculate position of desired bucket.
	cl_ulong calculated_position = (cl_long)std::floor(percentile * numbers_count);

	// Add numbers count which are under minimum of histogram to cumulative frequency.
	cl_ulong cumulative_frequency = numbers_count_under_min;
	cl_ulong position = 0;

	// Iterate through every bucket.
	for (cl_ulong i = 0; i < bucket_count; i++)
	{
		// Add bucket frequency to cumulitave frequency.
		cumulative_frequency += bucket_frequency[i];

		// Cumulative frequency must be more than calculated_position from percentile to actually find result bucket.
		if (cumulative_frequency > calculated_position)
		{
			break;
		}

		position++;
	}

	return position;
}

void set_min_max_file_value(std::vector<MIN_MAX_FILE_VALUES>& values, cl_double* values_file, cl_ulong* values_file_position, cl_ulong values_count, cl_ulong values_file_count)
{
	for (cl_ulong i = 0; i < values_file_count; i++)
	{
		cl_ulong value_file_position = values_file_position[i];
		if (values[value_file_position].min_value_file > values_file[i])
		{
			values[value_file_position].min_value_file = values_file[i];
		}

		if (values[value_file_position].max_value_file < values_file[i])
		{
			values[value_file_position].max_value_file = values_file[i];
		}
	}
}
 
cl_double test() {
	
	// TODO: check OpenCL platform from arguments.	
	
	cl_long result = 0;
	
	// Get platform.
	cl::Platform default_platform;
	result = get_default_platform(default_platform);
	
	if (result > 0)
	{
		return 1;
	}
	
	// Get device.
	std::vector<cl::Device> all_devices;
	cl::Device default_device;
	result = get_all_devices(all_devices, default_platform);

	if (result > 0)
	{
		return 1;
	}

	// Get default device.
	default_device = all_devices[0];
	
	// Create context and kernel program.
	cl::Context ctx = cl::Context(default_device);
	cl::CommandQueue queue = cl::CommandQueue(ctx, default_device);
	cl::Program program(ctx, kernel_src);
	
	// Build kernel program.
	cl_int build_status = program.build(all_devices);
	std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device);
	
	// Check build status.
	if (build_status != CL_SUCCESS) 
	{
		std::cerr << "Could not build OpenCL kernel:\n" << build_log  << std::endl;
		return 3;
	} 
	else if (!build_log.empty() && !std::all_of(build_log.begin(), build_log.end(), ::isspace)) 
	{
		std::cerr << "OpenCL kernel build output:\n" << build_log << std::endl;
	}

	// Create kernel function	
	cl::make_kernel<cl::Buffer, // min_bucket
					cl::Buffer, // max_bucket
					cl::Buffer, // frequency
					cl::Buffer, // numbers_count
					cl::Buffer, // numbers_count_under_min
					cl::Buffer, // value_file
					cl::Buffer, // value_file_position
					cl::Buffer, // data_buffer
					cl_double,	// min
					cl_double,	// max
					cl_ulong
					> process_data_block(cl::Kernel(program, "process_data_block"));

	cl_double min = std::numeric_limits<cl_double>::lowest();
	cl_double max = std::numeric_limits<cl_double>::max();
	
	std::ifstream stream("C:\\Users\\danisik\\Desktop\\PPR\\semestralka\\semestralni_prace\\party.mp3", std::ios::in | std::ios::binary);
	
	while (true) 
	{	
	    stream.clear();
        stream.seekg(0);
			
		// Create histogram.	
		cl_double bucket_min[BUCKET_COUNT_OPENCL];
		cl_double bucket_max[BUCKET_COUNT_OPENCL];
		cl_ulong bucket_frequency[BUCKET_COUNT_OPENCL];

		std::vector<MIN_MAX_FILE_VALUES> vector_bucket_value_file;
		
		// Counters.
		cl_ulong numbers_count = 0;
		cl_ulong numbers_count_under_min = 0;
		
		// Create buffers for histogram.
		const cl_long null_value = 0;
		
		cl::Buffer buffer_bucket_min(ctx, CL_MEM_READ_WRITE, BUCKET_COUNT_OPENCL);
		cl::Buffer buffer_bucket_max(ctx, CL_MEM_READ_WRITE, BUCKET_COUNT_OPENCL);
		cl::Buffer buffer_bucket_frequency (ctx, CL_MEM_READ_WRITE, BUCKET_COUNT_OPENCL);
		
		cl::Buffer buffer_numbers_count (ctx, CL_MEM_READ_WRITE, sizeof(cl_ulong));
		cl::Buffer buffer_numbers_count_under_min (ctx, CL_MEM_READ_WRITE, sizeof(cl_ulong));
		
		// TODO: remove in final version		
		cl_double percentile = 0.00;
		
		// Buffer vars.
		cl_double buffer[BLOCK_SIZE_OPENCL / sizeof(cl_double)];
		cl::Buffer buffer_data_block(ctx, CL_MEM_READ_WRITE, BLOCK_SIZE_OPENCL / sizeof(buffer));
		
		cl_long offset = 0;
		cl_long read_count = 0;

		// Set default values to buffers.
		std::memset(bucket_frequency, 0, sizeof(bucket_frequency));

		// Calculate step.
		cl_double step = std::abs(min / BUCKET_COUNT_OPENCL) + (max / BUCKET_COUNT_OPENCL);

		cl_double from = min;
		cl_double to = min + step;

		// Create buckets and set range.
		for (size_t i = 0; i < BUCKET_COUNT_OPENCL; i++)
		{
			if (i == BUCKET_COUNT_OPENCL - 1) to = max;
			bucket_min[i] = from;
			bucket_max[i] = to;

			MIN_MAX_FILE_VALUES values;
			values.min_value_file = std::numeric_limits<cl_double>::max();
			values.max_value_file = std::numeric_limits<cl_double>::lowest();

			vector_bucket_value_file.push_back(values);

			from = to;
			to += step;
		}
		
		// Provide buffers to GPU.
		queue.enqueueWriteBuffer(buffer_bucket_min, CL_FALSE, 0, BUCKET_COUNT_OPENCL * sizeof(cl_double), bucket_min);
		queue.enqueueWriteBuffer(buffer_bucket_max, CL_FALSE, 0, BUCKET_COUNT_OPENCL * sizeof(cl_double), bucket_max);
		queue.enqueueWriteBuffer(buffer_bucket_frequency, CL_FALSE, 0, BUCKET_COUNT_OPENCL * sizeof(cl_ulong), bucket_frequency);
		queue.enqueueWriteBuffer(buffer_numbers_count, CL_FALSE, 0, sizeof(cl_ulong), &null_value);
		queue.enqueueWriteBuffer(buffer_numbers_count_under_min, CL_FALSE, 0, sizeof(cl_ulong), &null_value);

		// Read file.
		while (true)
		{			
			// Read block of data.		
			stream.seekg(offset * BLOCK_SIZE_OPENCL);
			
			stream.read(reinterpret_cast<char*>(buffer), BLOCK_SIZE_OPENCL);

			offset++;
			
			// If nothing was read.
			if (stream.gcount() == 0)
			{
				break;
			}			
			
			// Stream read count.
			read_count = stream.gcount() / sizeof(cl_double);		
			
			cl::EnqueueArgs eargs(queue, cl::NDRange(read_count));
			
			// Enqueue data.
			queue.enqueueWriteBuffer(buffer_data_block, CL_TRUE, 0, sizeof(buffer), buffer);

			// Initialize buffer for values and position.
			cl_double bucket_value_file[BLOCK_SIZE_OPENCL];
			cl_ulong bucket_value_file_position[BLOCK_SIZE_OPENCL];
			cl::Buffer buffer_bucket_value_file(ctx, CL_MEM_READ_WRITE, BLOCK_SIZE_OPENCL);
			cl::Buffer buffer_bucket_value_file_position(ctx, CL_MEM_READ_WRITE, BLOCK_SIZE_OPENCL);

			// Set default values.
			std::memset(bucket_value_file, 0, sizeof(bucket_value_file));
			std::memset(bucket_value_file_position, 0, sizeof(bucket_value_file_position));

			// Enqueue buffers to GPU.
			queue.enqueueWriteBuffer(buffer_bucket_value_file, CL_FALSE, 0, BUCKET_COUNT_OPENCL * sizeof(cl_double), bucket_value_file);
			queue.enqueueWriteBuffer(buffer_bucket_value_file_position, CL_FALSE, 0, BUCKET_COUNT_OPENCL * sizeof(cl_ulong), bucket_value_file_position);

			// Call function.
			process_data_block(eargs, buffer_bucket_min, buffer_bucket_max, buffer_bucket_frequency, buffer_numbers_count, 
								buffer_numbers_count_under_min, buffer_bucket_value_file, buffer_bucket_value_file_position,
								buffer_data_block, min, max, BUCKET_COUNT_OPENCL).wait();

			// Read buffers.
			queue.enqueueReadBuffer(buffer_bucket_value_file, CL_TRUE, 0, BUCKET_COUNT_OPENCL * sizeof(cl_double), &bucket_value_file);
			queue.enqueueReadBuffer(buffer_bucket_value_file_position, CL_TRUE, 0, BUCKET_COUNT_OPENCL * sizeof(cl_ulong), &bucket_value_file_position);

			// Set min and max values.
			set_min_max_file_value(vector_bucket_value_file, bucket_value_file, bucket_value_file_position, BUCKET_COUNT_OPENCL, BLOCK_SIZE_OPENCL);
		}
		
		// Get buffers from GPU.
		queue.enqueueReadBuffer(buffer_bucket_min, CL_FALSE, 0, BUCKET_COUNT_OPENCL * sizeof(cl_double), bucket_min);
		queue.enqueueReadBuffer(buffer_bucket_max, CL_FALSE, 0, BUCKET_COUNT_OPENCL * sizeof(cl_double), bucket_max);
		queue.enqueueReadBuffer(buffer_bucket_frequency, CL_FALSE, 0, BUCKET_COUNT_OPENCL * sizeof(cl_ulong), &bucket_frequency);
		queue.enqueueReadBuffer(buffer_numbers_count, CL_FALSE, 0, sizeof(cl_ulong), &numbers_count);
		queue.enqueueReadBuffer(buffer_numbers_count_under_min, CL_FALSE, 0, sizeof(cl_ulong), &numbers_count_under_min);
		
		queue.finish();

		// Find position of bucket based on percentile.
		cl_ulong position = find_bucket_opencl(bucket_frequency, percentile, numbers_count, numbers_count_under_min, BUCKET_COUNT_OPENCL);

		cl_double found_min = vector_bucket_value_file[position].min_value_file;
		cl_double found_max = vector_bucket_value_file[position].max_value_file;

		if (found_min == found_max)
		{
			std::cout << "kokoot " << found_min << std::endl;
			return found_min;
		}
	}
}