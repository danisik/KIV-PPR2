#include "main.h"

/// <summary>
/// Main method.
/// </summary>
/// <param name="argc">Arguments count</param>
/// <param name="argv">Arguments buffer</param>
/// <returns>Exit code</returns>
int wmain(int argc, wchar_t** argv)
{
	/*
	- Parametry: pprsolver.exe soubor percentil procesor
	soubor - cesta k souboru, m��e b�t relativn� k program.exe, ale i absolutn�
		percentil - ��slo 1 - 100
		procesor - �et�zec ur�uj��c�, na jak�m procesoru a jak v�po�et prob�hne
			single - jednovl�knov� v�po�et na CPU
			SMP - v�cevl�knov� v�po�et na CPU
			anebo n�zev OpenCL za��zen� - pozor, v syst�mu m��e b�t n�kolik OpenCL platforem


	- ��sla ze souboru: 64-bitov� double


	- Jako ��sla budete uva�ovat pouze ty 8-bytov� sekvence, pro kter� std::fpclassify vr�t� FP_NORMAL nebo FP_ZERO.
		Jin� sekvence budete ignorovat. V�echny nuly jsou si stejn� rovn�.


	- Pam� bude omezen� na 250 MB


	- Program mus� skon�it do 15 minut


	- Explicitn� upozorn�n� dle postupn�ch dotaz� z �ad student�:
		- Program nebude m�t povoleno vytv��et soubory na disku.
		- Jako ��sla budete uva�ovat pouze ty 8-bytov� sekvence, pro kter� std::fpclassify vr�t� FP_NORMAL nebo FP_ZERO.
			Jin� sekvence budete ignorovat. V�echny nuly jsou si stejn� rovn�.
		- Pozice v souboru vypisujte v bytech, tj. indexov�no od nuly.
		- Hexadecim�ln� ��slo vypi�te nap�. pomoc� std::hexfloat.


		TODO: OpenCL CUDA https://medium.com/@pratikone/opencl-on-visual-studio-configuration-tutorial-for-the-confused-3ec1c2b5f0ca
	*/

	// Check if there is 3 arguments.
	if (argc != 4)
	{
		std::wcout << "Invalid number of arguments - need 3 arguments." << std::endl;
		std::wcout << "Usage: pprsolver.exe <path_to_binary_file> <percentile> <processor>" << std::endl;
		std::wcout << "<path_to_binary_file> - Full or relative path to binary file." << std::endl;
		std::wcout << "<percentile> - Percentile value in %. Value between 0-100." << std::endl;
		std::wcout << "<processor> - Used processor. Available processors: 'single', 'SMP' or any opencl platform name installed on pc." << std::endl;
		return EXIT_CODE::INVALID_ARGS;
	}

	// File path argument.
	std::wstring file_path_WS(argv[1]);
	std::string file_path;

	std::transform(file_path_WS.begin(), file_path_WS.end(), std::back_inserter(file_path), [](wchar_t c) {
		return (char)c;
		});

	// Check if file path is valid.
	FILE* p_file;
	fopen_s(&p_file, file_path.c_str(), "r");
	if (p_file == NULL)
	{
		std::wcout << "Binary file did not exists, please provide valid path to file." << std::endl;
		return EXIT_CODE::INVALID_ARGS;
	}

	// Percentile argument.
	std::wstring percentile_WS(argv[2]);
	std::string percentile_string;

	std::transform(percentile_WS.begin(), percentile_WS.end(), std::back_inserter(percentile_string), [](wchar_t c) {
		return (char)c;
		});

	// Convert string to double.
	auto converted_percentile = std::istringstream(percentile_string);
	double percentile = -1;

	converted_percentile >> percentile;

	// If conversion failed, then exit app.
	if (converted_percentile.fail() || !converted_percentile.eof())
	{
		std::wcout << "Percentile is not a number." << std::endl;
		return EXIT_CODE::INVALID_ARGS;
	}

	// Check if percentile value is between 0 and 100.
	if (percentile < 0)
	{
		std::wcout << "Percentile too low, it must be between 0 and 100." << std::endl;
		return EXIT_CODE::INVALID_ARGS;
	}
	else if (percentile > 100)
	{
		std::wcout << "Percentile too high, it must be between 0 and 100." << std::endl;
		return EXIT_CODE::INVALID_ARGS;
	}

	// Work with 0-1 value.
	percentile /= 100;

	// Processor argument.
	std::wstring processor_WS(argv[3]);
	std::string processor;

	std::transform(processor_WS.begin(), processor_WS.end(), std::back_inserter(processor), [](wchar_t c) {
		return (char)c;
		});

	// To be sure, lower all characters in processor variable.
	std::string processor_lower = Utils::to_lower(processor);

	// Set first min and max.
	double min = std::numeric_limits<double>::lowest();
	double max = std::numeric_limits<double>::max();
	Histogram_Object result_bucket;

	auto t_start = std::chrono::high_resolution_clock::now();
	int cycles = 0;

	// TODO: remove.
	processor_lower = "opencl";
	file_path = "C:\\Users\\danisik\\Desktop\\PPR\\semestralka\\semestralni_prace\\party.mp3";
	percentile = 0.4;

	// Open stream to file.
	std::ifstream stream(file_path, std::ios::in | std::ios::binary);

	// Check if file exists and stream is open.
	if (!(stream && stream.is_open()))
	{
		std::wcout << "Cannot open file: " << file_path.c_str() << std::endl;
		return EXIT_CODE::INVALID_FILE;
	}

	// Create watchdog thread.
	watchdog.start();

	std::vector<Histogram_Object> buckets = create_buckets(min, max);

	// Return buckets until only single number is presented.
	while (true)
	{
		if (processor_lower == "single")
		{
			result_bucket = get_bucket(stream, buckets, percentile, min, max);
		}
		else if (processor_lower == "smp")
		{
			result_bucket = get_bucket_SMP(stream, buckets, percentile, min, max);
		}
		else
		{
			// TODO: opencl in progress
			// TODO: based on return value print invalid args or do job.
			/*
			std::cout << "Invalid processor type - " << processor << std::endl;
			std::cout << "Allowed values: 'single', 'SMP' or any opencl platform name installed on pc." << std::endl;
			*/
			// Wait for watchdog.			
			test();
			system("pause");
			watchdog.join();
			return EXIT_CODE::INVALID_ARGS;
		}

		// Check if calculation is cycled or not.
		if (min == result_bucket.get_min_value_file() && max == result_bucket.get_max_value_file())
		{
			watchdog.stop();
			std::cout << "Stopping program, calculation was cycled." << std::endl;
			return EXIT_CODE::CYCLED_CALCULATION;
		}
		else
		{
			watchdog.reset();
		}

		// Get minimum found value in file for bucket.
		min = result_bucket.get_min_value_file();
		// Get maximum found value in file for bucket.
		max = result_bucket.get_max_value_file();

		reset_buckets(buckets, min, max);

		cycles++;

		// If bucket is represented as single number, then we found what we want.
		if (min == max)
		{

			std::wcout << "Input parameters:" << std::endl;
			std::wcout << "--------------------" << std::endl;
			std::wcout << "- File: " << file_path.c_str() << std::endl;
			std::wcout << "- Percentile: " << percentile * 100 << "%" << std::endl;
			std::wcout << "- Processor: " << processor.c_str() << std::endl;
			std::wcout << "--------------------" << std::endl;
			std::wcout << std::endl;

			std::wcout << "Output" << std::endl;
			std::wcout << "--------------------" << std::endl;
			std::wcout << "- Cycles: " << cycles << std::endl;
			std::wcout << "- Value [HEX]: " << std::hexfloat << result_bucket.get_min_value_file() << std::endl;
			std::wcout << "- Frequency: " << std::fixed << result_bucket.get_frequency() << std::endl;
			std::wcout << "--------------------" << std::endl;

			break;
		}
	}

	if (processor_lower != "opencl")
	{
		// Iterate through file to find first and last position of number.
		NUMBER_POSITION position;
		position.first_occurence = std::numeric_limits<size_t>::max();
		position.last_occurence = std::numeric_limits<size_t>::lowest();
		bool found = get_number_positions(stream, min, position);

		// TODO: v�stup
		//std::wcout << std::hexfloat << result_bucket.get_min_value_file() << " " << std::fixed << position.first_occurence << " " << position.last_occurence << std::endl;


		// Print position.
		std::wcout << std::endl;
		std::wcout << "Value position" << std::endl;
		std::wcout << "--------------------" << std::endl;
		std::wcout << "- Position found: " << found << std::endl;
		std::wcout << "- First position [BYTE]: " << position.first_occurence << std::endl;
		std::wcout << "- Last position [BYTE]: " << position.last_occurence << std::endl;
		std::wcout << "--------------------" << std::endl;


		// Print time.
		auto t_end = std::chrono::high_resolution_clock::now();
		double elapsed_time = std::chrono::duration<double, std::milli>(t_end - t_start).count() / 1000;
		std::wcout << std::endl;
		std::wcout << "Time needed: " << std::fixed << elapsed_time << " sec" << std::endl;
	}

	// Close file.
	stream.close();

	// Wait for watchdog.
	watchdog.join();

	system("pause");
	return EXIT_CODE::SUCCESS;
}

/// <summary>
/// Get bucket based on percentile for single CPU.
/// </summary>
/// <param name="stream">File stream</param>
/// <param name="percentile">Percentile</param>
/// <param name="min">Min value for histogram</param>
/// <param name="max">Max value for histogram</param>
/// <returns>Bucket</returns>
Histogram_Object get_bucket(std::ifstream& stream, std::vector<Histogram_Object>& buckets, double percentile, double min, double max)
{
	watchdog.reset();

	// Seek stream to start.
	stream.clear();
	stream.seekg(0);

	// Init return vals.
	Histogram_Object result_bucket;

	size_t i = 0;
	size_t numbers_count = 0;
	size_t numbers_count_under_min = 0;

	uint64_t offset = 0;

	// Create buffer on heap, because it is too large to save on stack.
	double* buffer = new double[BLOCK_SIZE / sizeof(double)];

	watchdog.reset();
	// Read file.
	while (true)
	{
		// Read block of data.
		stream.seekg(offset * BLOCK_SIZE);
		stream.read(reinterpret_cast<char*>(buffer), BLOCK_SIZE);

		offset++;

		// If nothing was read.
		if (stream.gcount() == 0)
		{
			break;
		}
		size_t read_count = stream.gcount() / sizeof(double);

		// Process data block -> find values + count numbers.
		COUNTER_OBJECT counter_obj = process_data_block(buckets, buffer, read_count, min, max);

		numbers_count += counter_obj.numbers_count;
		numbers_count_under_min += counter_obj.numbers_count_under_min;

		watchdog.reset();
	}

	// Find bucket based on percentile.
	result_bucket = find_bucket(buckets, numbers_count, numbers_count_under_min, percentile);

	delete[] buffer;
	watchdog.reset();
	return result_bucket;
}

/// <summary>
/// Get bucket based on percentile for SMP.
/// </summary>
/// <param name="stream">File stream</param>
/// <param name="percentile">Percentile</param>
/// <param name="min">Minimum allowed value for histogram</param>
/// <param name="max">Maximum allowed value for histogram</param>
/// <returns>Bucket</returns>
Histogram_Object get_bucket_SMP(std::ifstream& stream, std::vector<Histogram_Object>& buckets, double percentile, double min, double max)
{
	watchdog.reset();

	// Reset queue shutdown variable.
	queue.restart_shutdown();

	// Seek stream to start.
	stream.clear();
	stream.seekg(0);

	// Create threads.
	std::vector<std::thread> threads;
	std::vector<HISTOGRAM> thread_histograms;

	// Create histograms for threads. Must be done before creating threads, because there was some problem with references.
	for (unsigned int i = 0; i < THREADS_COUNT; i++)
	{
		thread_histograms.emplace_back();
	}

	// Create histograms and assign them to threads.
	for (unsigned int i = 0; i < THREADS_COUNT; i++)
	{
		threads.emplace_back(create_sub_histogram, std::ref(thread_histograms[i]), min, max);
	}

	watchdog.reset();
	uint64_t offset = 0;

	// Read file.
	while (true)
	{
		// Create buffer on heap, because it is too large to save on stack.
		double* buffer = new double[BLOCK_SIZE / sizeof(double)];

		// Read block of data.
		stream.seekg(offset * BLOCK_SIZE);
		stream.read(reinterpret_cast<char*>(buffer), BLOCK_SIZE);

		offset++;

		// If nothing was read.
		if (stream.gcount() == 0)
		{
			delete[] buffer;
			break;
		}
		size_t read_count = stream.gcount() / sizeof(double);

		// Create buffer object.
		BUFFER_OBJECT buffer_object;
		buffer_object.buffer = buffer;
		buffer_object.read_count = read_count;

		// Push buffer with data to queue.
		queue.push(buffer_object);

		watchdog.reset();
	}

	// Request shutdown for all threads
	queue.request_shutdown();

	// Wait for all threads to end.
	for (std::thread& th : threads)
	{
		th.join();
	}

	Histogram_Object result_bucket;

	size_t numbers_count = 0;
	size_t numbers_count_under_min = 0;

	watchdog.reset();
	// Merge histograms.
	for (int j = 0; j < THREADS_COUNT; j++)
	{
		HISTOGRAM& thread_histogram = thread_histograms[j];

		// Increment counters.
		numbers_count += thread_histogram.numbers_count;
		numbers_count_under_min += thread_histogram.numbers_count_under_min;

		std::vector<Histogram_Object> thread_buckets = thread_histogram.buckets;

		watchdog.reset();
		for (int i = 0; i < buckets.size(); i++)
		{
			// Get bucket from main histogram and from thread histogram.
			Histogram_Object& mainBucket = buckets[i];
			Histogram_Object& threadBucket = thread_buckets[i];

			// Set / Increment values.
			mainBucket.add_frequency(threadBucket.get_frequency());
			mainBucket.set_max_value_file(threadBucket.get_max_value_file());
			mainBucket.set_min_value_file(threadBucket.get_min_value_file());
		}
	}

	// Find bucket based on percentile.
	result_bucket = find_bucket(buckets, numbers_count, numbers_count_under_min, percentile);

	watchdog.reset();
	return result_bucket;
}

/// <summary>
/// Get first and last position of value.
/// </summary>
/// <param name="stream">File stream.</param>
/// <param name="desired_value">Value to be found in file.</param>
/// <param name="position">Reference to object representing first and last position.</param>
/// <returns>True if getting positions was successful, false if not.</returns>
bool get_number_positions(std::ifstream& stream, double desired_value, NUMBER_POSITION& position)
{
	watchdog.reset();
	// Create buffer on heap, because it is too large to save on stack.
	double* buffer = new double[BLOCK_SIZE / sizeof(double)];

	bool first_occurence_changed = false;
	bool last_occurence_changed = false;
	size_t counter = 0;

	// Seek stream to start.
	stream.clear();
	stream.seekg(0);

	uint64_t offset = 0;

	// Read file.
	while (true)
	{
		// Read block of data.
		stream.seekg(offset * BLOCK_SIZE);
		stream.read(reinterpret_cast<char*>(buffer), BLOCK_SIZE);

		offset++;

		// If nothing was read.
		if (stream.gcount() == 0)
		{
			break;
		}

		size_t read_count = stream.gcount() / sizeof(double);

		watchdog.reset();
		for (size_t i = 0; i < read_count; i++)
		{
			double value = buffer[i];

			if (value == desired_value)
			{
				if (position.first_occurence >= counter)
				{
					position.first_occurence = counter;
					first_occurence_changed = true;
				}

				if (position.last_occurence <= counter)
				{
					position.last_occurence = counter;
					last_occurence_changed = true;
				}
			}

			counter++;
		}
	}

	// We want position in Bytes.
	position.first_occurence *= CUSTOM_BYTE;
	position.last_occurence *= CUSTOM_BYTE;

	// Delete buffer from heap.
	delete[] buffer;

	watchdog.reset();
	// If occurences were changed, return true.
	if (first_occurence_changed && last_occurence_changed)
		return true;

	return false;
}

/// <summary>
/// Create histogram (vector with buckets).
/// </summary>
/// <param name="min">Minimum allowed value of histogram.</param>
/// <param name="max">Maximum allowed value of histogram.</param>
/// <returns>Vector with buckets.</returns>
std::vector<Histogram_Object> create_buckets(double min, double max)
{
	std::vector<Histogram_Object> buckets;

	// Calculate step.
	double step = std::abs(min / BUCKET_COUNT) + (max / BUCKET_COUNT);
		
	// Set from, to.
	double from = min;
	double to = min + step;

	// Create buckets and set range.
	for (size_t i = 0; i < BUCKET_COUNT; i++)
	{
		if (i == BUCKET_COUNT - 1) to = max;
		Histogram_Object object(from, to);
		buckets.push_back(object);

		from = to;
		to += step;
	}

	return buckets;
}

/// <summary>
/// Create histogram (vector with buckets).
/// </summary>
/// <param name="min">Minimum allowed value of histogram.</param>
/// <param name="max">Maximum allowed value of histogram.</param>
/// <returns>Vector with buckets.</returns>
void reset_buckets(std::vector<Histogram_Object>& buckets, double min, double max)
{
	// Calculate step.
	double step = std::abs(min / BUCKET_COUNT) + (max / BUCKET_COUNT);

	// Set from, to.
	double from = min;
	double to = min + step;

	// Create buckets and set range.
	for (size_t i = 0; i < BUCKET_COUNT; i++)
	{
		if (i == BUCKET_COUNT - 1) to = max;
		buckets[i].set_min(from);
		buckets[i].set_max(to);
		buckets[i].reset_values();

		from = to;
		to += step;
	}
}

/// <summary>
/// Process single data block -> get value, get position of bucket and assing value to bucket.
/// </summary>
/// <param name="buckets">Histogram.</param>
/// <param name="buffer">Data buffer.</param>
/// <param name="read_count">Data read count.</param>
/// <param name="min">Minimum allowed value of histogram.</param>
/// <param name="max">Maximum allowed value of histogram.</param>
/// <returns>Object representing number counts processed.</returns>
COUNTER_OBJECT process_data_block(std::vector<Histogram_Object>& buckets, double* buffer, size_t read_count, double min, double max)
{
	COUNTER_OBJECT obj;

	for (size_t i = 0; i < read_count; i++)
	{
		watchdog.reset();
		double value = buffer[i];

		// Check if double value is correct value.
		if (Utils::is_correct_value(value))
		{
			// Check if value is in histogram range.
			if (value <= max && value >= min)
			{
				// Fix for last bucket -> if value is = max, then it means it must be in the next bucket, because 
				// we want <min, max) and we will throw away values = max when applying for last bucket.
				int addition = 0;
				if (value == max)
				{
					addition = 1;
				}

				// Get position using binary search.
				long position = Utils::binary_search(buckets, 0, BUCKET_COUNT - 1, value);

				watchdog.reset();
				// If position was found.
				if (position != -1)
				{
					if (position == (BUCKET_COUNT - 1))
						addition = 0;

					Histogram_Object& bucket = buckets[position + addition];
					bucket.increment_frequency();
					bucket.set_min_value_file(value);
					bucket.set_max_value_file(value);
				}
				else
				{
					Histogram_Object& bucket = buckets[buckets.size() - 1];
					bucket.increment_frequency();
					bucket.set_min_value_file(value);
					bucket.set_max_value_file(value);
				}
			}
			else if (value < min)
			{
				// Increment counter for values under minimum.
				obj.numbers_count_under_min++;
			}

			// Increment total counts.
			obj.numbers_count++;
		}
	}

	watchdog.reset();
	return obj;
}

/// <summary>
/// Find bucket based on percentile.
/// </summary>
/// <param name="buckets">Histogram.</param>
/// <param name="numbers_count">Numbers count in histogram.</param>
/// <param name="numbers_count_under_min">Numbers count under minimum of histogram.</param>
/// <param name="percentile">Percentile.</param>
/// <returns>Bucket.</returns>
Histogram_Object find_bucket(std::vector<Histogram_Object> buckets, size_t numbers_count, size_t numbers_count_under_min, double percentile)
{
	watchdog.reset();
	Histogram_Object result_bucket;

	// Calculate position of desired bucket.
	size_t calculated_position = (size_t)std::floor(percentile * numbers_count);

	// Add numbers count which are under minimum of histogram to cumulative frequency.
	size_t cumulative_frequency = numbers_count_under_min;
	size_t position = 0;

	// If percentil is 100%.
	if (calculated_position == numbers_count)
	{
		// Reverse iterate through histogram - we need to find last bucket with frequency -> that is 100% of histogram.
		position = BUCKET_COUNT - 1;
		for (std::vector<Histogram_Object>::reverse_iterator it = buckets.rbegin(); it != buckets.rend(); ++it)
		{
			Histogram_Object& obj = *it;
			if (obj.get_frequency() > 0)
			{
				result_bucket = obj;
				break;
			}

			position--;
		}
	}
	else
	{
		// Iterate through every bucket.
		for (Histogram_Object& bucket : buckets)
		{
			watchdog.reset();
			// Add bucket frequency to cumulitave frequency.
			cumulative_frequency += bucket.get_frequency();

			// Cumulative frequency must be more than calculated_position from percentile to actually find result bucket.
			if (cumulative_frequency > calculated_position)
			{
				break;
			}

			position++;
		}

		result_bucket = buckets[position];
	}

	watchdog.reset();
	return result_bucket;
}

/// <summary>
/// Method for thread. Same as finding bucket when using single processor, but with less data.
/// </summary>
/// <param name="histogram">Histogram object.</param>
/// <param name="min">Minimum allowed value of histogram.</param>
/// <param name="max">Maximum allowed value of histogram.</param>
void create_sub_histogram(HISTOGRAM& histogram, double min, double max)
{
	std::vector<Histogram_Object> buckets = create_buckets(min, max);
	size_t numbers_count = 0;
	size_t numbers_count_under_min = 0;

	while (true)
	{
		// Get block of data and process it.
		BUFFER_OBJECT buffer;
		bool popped = queue.pop(buffer);

		// If there is no free block to process and reading file was ended, end thread.
		if (!popped)
		{
			break;
		}
		else
		{
			watchdog.reset();
			// Process data block -> find values + count numbers.
			COUNTER_OBJECT counter_obj = process_data_block(buckets, buffer.buffer, buffer.read_count, min, max);

			numbers_count += counter_obj.numbers_count;
			numbers_count_under_min += counter_obj.numbers_count_under_min;

			delete[] buffer.buffer;
		}
	}

	// Assign buckets and counters to histogram struct.
	histogram.buckets = buckets;
	histogram.numbers_count = numbers_count;
	histogram.numbers_count_under_min = numbers_count_under_min;

	watchdog.reset();
}