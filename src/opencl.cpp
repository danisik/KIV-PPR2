#include "cl.h"

#undef max
 
/// <summary>
/// Get all available platforms on current pc.
/// </summary>
/// <param name="all_platforms">Empty vector.</param>
/// <returns>0 if there was at least 1 plaftorm, 1 if not.</returns>
long get_all_platforms_opencl(std::vector<cl::Platform>& all_platforms)
{
	// Get all platforms (drivers)
    cl::Platform::get(&all_platforms);
	
    if(all_platforms.size() == 0)
	{
        std::cout << "No platforms found. Check OpenCL installation!" << std::endl;
        return 1;
    }
	
	return 0;
}

/// <summary>
/// Get all available devices on current pc.
/// </summary>
/// <param name="all_devices">Empty vector.</param>
/// <param name="default_platform">Selected platform.</param>
/// <returns>0 if there was at least 1 plaftorm, 1 if not.</returns>
long get_all_devices_opencl(std::vector<cl::Device>& all_devices, cl::Platform default_platform)
{
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if(all_devices.size() == 0)
	{
        std::cout<<"No devices found. Check OpenCL installation!" << std::endl;
        return 1;
    }
	
	return 0;
}

/// <summary>
/// Find bucket position.
/// </summary>
/// <param name="bucket_frequency">Array of bucket frequencies.</param>
/// <param name="percentile">Percentile</param>
/// <param name="numbers_count">Numbers count.</param>
/// <param name="numbers_count_under_min">Numbers count uner minimum.</param>
/// <returns>Position of bucket on selected percentile.</returns>
cl_ulong find_bucket_position_opencl(cl_ulong* bucket_frequency, double percentile, cl_ulong numbers_count, cl_ulong numbers_count_under_min)
{
	// Calculate position of desired bucket.
	cl_ulong calculated_position = (cl_long)std::floor(percentile * numbers_count);

	// Add numbers count which are under minimum of histogram to cumulative frequency.
	cl_ulong cumulative_frequency = numbers_count_under_min;
	cl_ulong position = 0;

	// Iterate through every bucket.
	for (cl_ulong i = 0; i < BUCKET_COUNT; i++)
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
 
/// <summary>
/// Get bucket based on percentile.
/// </summary>
/// <param name="stream">File stream to data.</param>
/// <param name="histogram">Histogram object.</param>
/// <param name="result_bucket">Bucket where result will be stored.</param>
/// <param name="percentile">Percentile.</param>
/// <param name="min">Minimal value</param>
/// <param name="max">Maximal value</param>
/// <param name="input_device">Selected device</param>
/// <returns></returns>
long get_bucket_opencl(std::ifstream& stream, HISTOGRAM& histogram, Histogram_Object& result_bucket, double percentile, int64_t min, int64_t max, std::string input_device)
{
	watchdog.reset();
	long result = 0;
	cl::Device default_device;
	
	// Get platform.
	std::vector<cl::Platform> all_plaftorms;
	cl::Platform default_platform;
	result = get_all_platforms_opencl(all_plaftorms);
	
	if (result > 0)
	{
		return EXIT_CODE::OPENCL_PLAFTORM_NOT_FOUND;
	}
	
	cl_bool device_found = false;
	std::vector<cl::Device> all_devices;

	// Check if selected platform is in available platforms.
	for (cl_ulong i = 0; i < all_plaftorms.size(); i++)
	{
		cl::Platform platform = all_plaftorms[i];

		// Get devices for current platform.
		result = get_all_devices_opencl(all_devices, platform);

		// Iterate through every device.
		for (cl::Device device : all_devices)
		{
			// Device name.
			std::string device_name = device.getInfo<CL_DEVICE_NAME>();

			// Check if input device is presented in current list of devices.
			if (Utils::to_lower(device_name).find(input_device) != std::string::npos)
			{
				device_found = true;
				default_platform = platform;
				default_device = device;
				break;
			}
		}

		if (device_found)
		{
			break;
		}

		all_devices.clear();

		if (result > 0)
		{
			return EXIT_CODE::OPENCL_DEVICE_NOT_FOUND;
		}
	}

	// If not, return.
	if (!device_found)
	{
		std::cout << "Input OpenCL device cannot be found on this pc." << std::endl;
		return EXIT_CODE::INVALID_OPENCL_DEVICE;
	}
	
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
	cl::make_kernel<cl::Buffer, // bucket_frequency
					cl::Buffer, // numbers_count,
					cl::Buffer, // numbers_count_under_min
					cl::Buffer, // data_buffer
					cl_long,	// min
					cl_long,	// max
					cl_long,	// bucket_size
					cl_long		// bucket_index_offset
					> process_data_value(cl::Kernel(program, "process_data_value"));
	
	stream.clear();
    stream.seekg(0);
			
	// Create histogram.	
	cl_ulong bucket_frequency[BUCKET_COUNT];
		
	// Counters.
	cl_ulong numbers_count = 0;
	cl_ulong numbers_count_under_min = 0;
		
	// Create buffers for histogram.
	const cl_long null_value = 0;

	cl::Buffer buffer_bucket_frequency (ctx, CL_MEM_READ_WRITE, BUCKET_COUNT * sizeof(cl_ulong));
	cl::Buffer buffer_numbers_count (ctx, CL_MEM_READ_WRITE, sizeof(cl_ulong));
	cl::Buffer buffer_numbers_count_under_min (ctx, CL_MEM_READ_WRITE, sizeof(cl_ulong));
		
	// Buffer vars.
	cl_double buffer[BLOCK_SIZE / sizeof(cl_double)];
	cl::Buffer buffer_data_block(ctx, CL_MEM_READ_WRITE, sizeof(buffer));
		
	cl_long offset = 0;
	cl_long read_count = 0;

	// Set default values to buffers.
	std::memset(bucket_frequency, 0, sizeof(bucket_frequency));
		
	// Provide buffers to GPU.
	queue.enqueueWriteBuffer(buffer_bucket_frequency, CL_FALSE, 0, BUCKET_COUNT * sizeof(cl_ulong), bucket_frequency);
	queue.enqueueWriteBuffer(buffer_numbers_count, CL_FALSE, 0, sizeof(cl_ulong), &null_value);
	queue.enqueueWriteBuffer(buffer_numbers_count_under_min, CL_FALSE, 0, sizeof(cl_ulong), &null_value);

	// Read file.
	while (true)
	{			
		watchdog.reset();
		// Read block of data.		
		stream.seekg(offset * BLOCK_SIZE);
			
		stream.read(reinterpret_cast<char*>(buffer), BLOCK_SIZE);

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

		// Call function.
		process_data_value(eargs, buffer_bucket_frequency, buffer_numbers_count, buffer_numbers_count_under_min, 
							buffer_data_block, min, max, histogram.bucket_size, histogram.bucket_index_offset).wait();
	}
		
	watchdog.reset();
	// Get buffers from GPU.
	queue.enqueueReadBuffer(buffer_bucket_frequency, CL_FALSE, 0, BUCKET_COUNT * sizeof(cl_ulong), &bucket_frequency);
	queue.enqueueReadBuffer(buffer_numbers_count, CL_FALSE, 0, sizeof(cl_ulong), &numbers_count);
	queue.enqueueReadBuffer(buffer_numbers_count_under_min, CL_FALSE, 0, sizeof(cl_ulong), &numbers_count_under_min);
		
	queue.finish();

	// Find position of bucket based on percentile.
	cl_ulong position = find_bucket_position_opencl(bucket_frequency, percentile, numbers_count, numbers_count_under_min);

	// Get result bucket.
	result_bucket = histogram.buckets[position];
	watchdog.reset();

	return EXIT_CODE::SUCCESS;
}