#include <memory>

#include "main.h"

/// <summary>
/// Main method.
/// </summary>
/// <param name="argc">Arguments count</param>
/// <param name="argv">Arguments buffer</param>
/// <returns>Exit code</returns>
int wmain(int argc, wchar_t** argv) {

	/*
	- Parametry: pprsolver.exe soubor percentil procesor
	soubor - cesta k souboru, m˘ûe b˝t relativnÌ k program.exe, ale i absolutnÌ
		percentil - ËÌslo 1 - 100
		procesor - ¯etÏzec urËujÌÌcÌ, na jakÈm procesoru a jak v˝poËet probÏhne
			single - jednovl·knov˝ v˝poËet na CPU
			SMP - vÌcevl·knov˝ v˝poËet na CPU
			anebo n·zev OpenCL za¯ÌzenÌ - pozor, v systÈmu m˘ûe b˝t nÏkolik OpenCL platforem


	- »Ìsla ze souboru: 64-bitov˝ double


	- Jako ËÌsla budete uvaûovat pouze ty 8-bytovÈ sekvence, pro kterÈ std::fpclassify vr·tÌ FP_NORMAL nebo FP_ZERO. 
		JinÈ sekvence budete ignorovat. Vöechny nuly jsou si stejnÏ rovnÈ.


	- PamÏù bude omezen· na 250 MB


	- Program musÌ skonËit do 15 minut


	- ExplicitnÌ upozornÏnÌ dle postupn˝ch dotaz˘ z ¯ad student˘:
		- Program nebude mÌt povoleno vytv·¯et soubory na disku.
		- Jako ËÌsla budete uvaûovat pouze ty 8-bytovÈ sekvence, pro kterÈ std::fpclassify vr·tÌ FP_NORMAL nebo FP_ZERO. 
			JinÈ sekvence budete ignorovat. Vöechny nuly jsou si stejnÏ rovnÈ.
		- Pozice v souboru vypisujte v bytech, tj. indexov·no od nuly.
		- Hexadecim·lnÌ ËÌslo vypiöte nap¯. pomocÌ std::hexfloat.


		TODO: OpenCL CUDA https://medium.com/@pratikone/opencl-on-visual-studio-configuration-tutorial-for-the-confused-3ec1c2b5f0ca
	*/

	// Check if there is 3 arguments.
	if (argc != 4)
	{
		std::wcout << "Invalid number of arguments - need 3 arguments." << std::endl;
		std::wcout << "Usage: pprsolver.exe <path_to_binary_file> <percentile> <processor>" << std::endl;
		std::wcout << "<path_to_binary_file> - Full or relative path to binary file." << std::endl;
		std::wcout << "<percentile> - Percentile value in %. Value between 0-100." << std::endl;
		std::wcout << "<processor> - Used processor. Available processors: single, SMP, OpenCL." << std::endl;
		return EXIT_CODE::INVALID_ARGS;
	}

	// File path argument.
	std::wstring filePathWS(argv[1]);
	std::string filePath;

	std::transform(filePathWS.begin(), filePathWS.end(), std::back_inserter(filePath), [](wchar_t c) {
		return (char)c;
	});	

	// Percentile argument.
	wchar_t* stopwcs;
	double percentile = wcstod(argv[2], &stopwcs) / 100;

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

	// Processor argument.
	std::wstring processorWS(argv[3]);
	std::string processor;

	std::transform(processorWS.begin(), processorWS.end(), std::back_inserter(processor), [](wchar_t c) {
		return (char)c;
	});

	// To be sure, lower all characters in processor variable.
	std::string processorLower = Utils::toLower(processor);

	// Just for testing purposes.
	//filePath = "C:\\Users\\danisik\\Desktop\\PPR\\semestralka\\semestralni_prace\\data.iso";
	//percentile = (double)1 / (double)100;
	//processor = "smp";	

	// Set first min and max.
	double min = std::numeric_limits<double>::lowest();
	double max = std::numeric_limits<double>::max();
	HistogramObject resultBucket;

	auto t_start = std::chrono::high_resolution_clock::now();
	int cycles = 0;

	// Open stream to file.
	std::ifstream stream(filePath, std::ios::in | std::ios::binary);

	if (!(stream && stream.is_open()))
	{
		std::wcout << "Binary file did not exists, please provide valid path to file." << std::endl;
		return EXIT_CODE::INVALID_ARGS;
	}

	// Return buckets until only single number is presented.
	while (true)
	{
		if (processor == "single")
		{
			resultBucket = getBucket(stream, percentile, min, max);
		}
		else if (processor == "smp")
		{
			resultBucket = getBucketSMP(stream, percentile, min, max);
		}
		else if (processor == "opencl")
		{
			// TODO: in progress.
			break;
		}
		else
		{
			std::cout << "Invalid processor type - " << processor << std::endl;
			std::cout << "Allowed values: single, SMP, OpenCL" << std::endl;
			return EXIT_CODE::INVALID_ARGS;
		}

		// Get minimum found value in file for bucket.
		min = resultBucket.getMinValueFile();
		// Get maximum found value in file for bucket.
		max = resultBucket.getMaxValueFile();

		cycles++;

		// If bucket is represented as single number, then we found what we want.
		if (min == max)
		{
			std::wcout << "Processor: " << processor.c_str() << std::endl;
			std::wcout << "Percentile: " << percentile << std::endl;
			std::wcout << "Percentile %: " << percentile * 100 << std::endl;
			std::wcout << "Cycles: " << cycles << std::endl;
			std::wcout << "Min: " << resultBucket.getMin() << std::endl;
			std::wcout << "Max: " << resultBucket.getMax() << std::endl;
			std::wcout << "Min File: " << resultBucket.getMinValueFile() << std::endl;
			std::wcout << "Max File: " << resultBucket.getMaxValueFile() << std::endl;
			std::wcout << "Min File hex: " << std::hexfloat << resultBucket.getMinValueFile() << std::endl;
			std::wcout << "Max File hex: " << std::hexfloat << resultBucket.getMaxValueFile() << std::endl;
			std::wcout << "Frequency: " << std::fixed << resultBucket.getFrequency() << std::endl;
			break;
		}
	}

	// Iterate through file to find first and last position of number.
	NUMBER_POSITION position;
	position.firstOccurence = std::numeric_limits<size_t>::max();
	position.lastOccurence = std::numeric_limits<size_t>::lowest();
	bool found = getNumberPositions(stream, min, position);

	std::wcout << std::endl;
	std::wcout << "Position found: " << found << std::endl;
	std::wcout << "First position: " << position.firstOccurence << std::endl;
	std::wcout << "Last position: " << position.lastOccurence << std::endl;


	auto t_end = std::chrono::high_resolution_clock::now();
	double elapsed_time = std::chrono::duration<double, std::milli>(t_end - t_start).count() / 1000;
	std::wcout << std::endl;
	std::wcout << "Time: " << std::fixed << elapsed_time << " sec" << std::endl;

	// Close file.
	stream.close();

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
HistogramObject getBucket(std::ifstream& stream, double percentile, double min, double max)
{
	// Create buffer on heap, because it is too large to save on stack.
	double* buffer = new double[BLOCKSIZE / sizeof(double)];

	// Seek stream to start.
	stream.clear();
	stream.seekg(0);

	// Init return vals.
	HistogramObject resultBucket;
	std::vector<HistogramObject> buckets = createBuckets(min, max);

	size_t i = 0;
	size_t numbersCount = 0;
	size_t numbersCountUnderMin = 0;

	uint64_t offset = 0;

	// Read file.
	while (true)
	{			
		// Read block of data.
		stream.seekg(offset * BLOCKSIZE);
		stream.read(reinterpret_cast<char*>(buffer), BLOCKSIZE);

		offset++;

		// If nothing was read.
		if (stream.gcount() == 0)
		{
			break;
		}
		size_t readCount = stream.gcount() / sizeof(double);

		// Process data block -> find values + count numbers.
		COUNTER_OBJECT counterObj = processDataBlock(buckets, buffer, readCount, min, max);

		numbersCount += counterObj.numbersCount;
		numbersCountUnderMin += counterObj.numbersCountUnderMin;
	}

	// Find bucket based on percentile.
	resultBucket = findBucket(buckets, numbersCount, numbersCountUnderMin, percentile);

	// Delete buffer from heap.
	delete[] buffer;

	return resultBucket;
}

/// <summary>
/// Get bucket based on percentile for SMP.
/// </summary>
/// <param name="stream">File stream</param>
/// <param name="percentile">Percentile</param>
/// <param name="min">Minimum allowed value for histogram</param>
/// <param name="max">Maximum allowed value for histogram</param>
/// <returns>Bucket</returns>
HistogramObject getBucketSMP(std::ifstream& stream, double percentile, double min, double max)
{
	// Reset queue shutdown variable.
	queue.RestartShutdown();

	// Seek stream to start.
	stream.clear();
	stream.seekg(0);
		
	// Create threads.
	std::vector<std::thread> threads;
	std::vector<HISTOGRAM> threadHistograms;

	// Create histograms for threads. Must be done before creating threads, because there was some problem with references.
	for (unsigned int i = 0; i < THREADS_COUNT; i++)
	{
		threadHistograms.emplace_back();
	}

	// Create histograms and assign them to threads.
	for (unsigned int i = 0; i < THREADS_COUNT; i++)
	{
		threads.emplace_back(createSubHistogram, std::ref(threadHistograms[i]), min, max);
	}

	uint64_t offset = 0;
	
	// Read file.
	while (true)
	{
		// Create buffer on heap, because it is too large to save on stack.
		double* buffer = new double[BLOCKSIZE / sizeof(double)];

		// Read block of data.
		stream.seekg(offset * BLOCKSIZE);
		stream.read(reinterpret_cast<char*>(buffer), BLOCKSIZE);

		offset++;

		// If nothing was read.
		if (stream.gcount() == 0)
		{
			delete[] buffer;
			break;
		}
		size_t readCount = stream.gcount() / sizeof(double);

		// Create buffer object.
		BUFFER_OBJECT bufferObject;
		bufferObject.buffer = buffer;
		bufferObject.readCount = readCount;

		// Push buffer with data to queue.
		queue.Push(bufferObject);
	}

	// Request shutdown for all threads
	queue.RequestShutdown();

	// Wait for all threads to end.
	for (std::thread& th : threads)
	{
		th.join();
	}

	HistogramObject resultBucket;
	std::vector<HistogramObject> buckets = createBuckets(min, max);

	size_t numbersCount = 0;
	size_t numbersCountUnderMin = 0;
	
	// Merge histograms.
	for (int j = 0; j < THREADS_COUNT; j++)
	{
		HISTOGRAM& threadHistogram = threadHistograms[j];

		// Increment counters.
		numbersCount += threadHistogram.numbersCount;
		numbersCountUnderMin += threadHistogram.numbersCountUnderMin;

		std::vector<HistogramObject> threadBuckets = threadHistogram.buckets;
		
		for (int i = 0; i < buckets.size(); i++)
		{
			// Get bucket from main histogram and from thread histogram.
			HistogramObject& mainBucket = buckets[i];
			HistogramObject& threadBucket = threadBuckets[i];

			// Set / Increment values.
			mainBucket.addFrequency(threadBucket.getFrequency());
			mainBucket.setMaxValueFile(threadBucket.getMaxValueFile());
			mainBucket.setMinValueFile(threadBucket.getMinValueFile());
		}
	}

	// Find bucket based on percentile.
	resultBucket = findBucket(buckets, numbersCount, numbersCountUnderMin, percentile);

	return resultBucket;
}

/// <summary>
/// Get first and last position of value.
/// </summary>
/// <param name="stream">File stream.</param>
/// <param name="desiredValue">Value to be found in file.</param>
/// <param name="position">Reference to object representing first and last position.</param>
/// <returns>True if getting positions was successful, false if not.</returns>
bool getNumberPositions(std::ifstream& stream, double desiredValue, NUMBER_POSITION& position)
{
	// Create buffer on heap, because it is too large to save on stack.
	double* buffer = new double[BLOCKSIZE / sizeof(double)];

	bool firstOccurenceChanged = false;
	bool lastOccurenceChanged = false;
	size_t counter = 0;

	// Seek stream to start.
	stream.clear();
	stream.seekg(0);

	uint64_t offset = 0;	

	// Read file.
	while (true)
	{
		// Read block of data.
		stream.seekg(offset * BLOCKSIZE);
		stream.read(reinterpret_cast<char*>(buffer), BLOCKSIZE);

		offset++;

		// If nothing was read.
		if (stream.gcount() == 0)
		{
			break;
		}

		size_t readCount = stream.gcount() / sizeof(double);

		for (size_t i = 0; i < readCount; i++)
		{
			double value = buffer[i];
			
			// Check if double value is correct value.
			if (Utils::isCorrectValue(value))
			{
				if (value == desiredValue)
				{
					if (position.firstOccurence >= counter)
					{
						position.firstOccurence = counter;
						firstOccurenceChanged = true;
					}

					if (position.lastOccurence <= counter)
					{					
						position.lastOccurence = counter;
						lastOccurenceChanged = true;
					}
				}
			}

			counter++;
		}
	}

	// We want position in Bytes.
	position.firstOccurence *= BYTE;
	position.lastOccurence *= BYTE;

	// Delete buffer from heap.
	delete[] buffer;

	// If occurences were changed, return true.
	if (firstOccurenceChanged && lastOccurenceChanged)
		return true;

	return false;
}

/// <summary>
/// Create histogram (vector with buckets).
/// </summary>
/// <param name="min">Minimum allowed value of histogram.</param>
/// <param name="max">Maximum allowed value of histogram.</param>
/// <returns>Vector with buckets.</returns>
std::vector<HistogramObject> createBuckets(double min, double max)
{
	std::vector<HistogramObject> buckets;

	// Calculate step.
	double step = std::abs(min / BUCKET_COUNT) + (max / BUCKET_COUNT);

	// Set from, to.
	double from = min;
	double to = min + step;

	// Create buckets and set range.
	for (size_t i = 0; i < BUCKET_COUNT; i++)
	{
		if (i == BUCKET_COUNT - 1) to = max;
		HistogramObject object(from, to);
		buckets.push_back(object);

		from = to;
		to += step;
	}

	return buckets;
}

/// <summary>
/// Process single data block -> get value, get position of bucket and assing value to bucket.
/// </summary>
/// <param name="buckets">Histogram.</param>
/// <param name="buffer">Data buffer.</param>
/// <param name="readCount">Data read count.</param>
/// <param name="min">Minimum allowed value of histogram.</param>
/// <param name="max">Maximum allowed value of histogram.</param>
/// <returns>Object representing number counts processed.</returns>
COUNTER_OBJECT processDataBlock(std::vector<HistogramObject>& buckets, double* buffer, size_t readCount, double min, double max)
{
	COUNTER_OBJECT obj;

	for (size_t i = 0; i < readCount; i++)
	{
		double value = buffer[i];

		// Check if double value is correct value.
		if (Utils::isCorrectValue(value))
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
				size_t position = Utils::binarySearch(buckets, 0, BUCKET_COUNT - 1, value);

				// If position was found.
				if (position != -1)
				{
					if (position == (BUCKET_COUNT - 1))
						addition = 0;

					HistogramObject& bucket = buckets[position + addition];
					bucket.incrementFrequency();
					bucket.setMinValueFile(value);
					bucket.setMaxValueFile(value);
				}
				else
				{
					HistogramObject& bucket = buckets[buckets.size() - 1];
					bucket.incrementFrequency();
					bucket.setMinValueFile(value);
					bucket.setMaxValueFile(value);
				}
			}
			else if (value < min)
			{
				// Increment counter for values under minimum.
				obj.numbersCountUnderMin++;
			}

			// Increment total counts.
			obj.numbersCount++;
		}
	}

	return obj;
}

/// <summary>
/// Find bucket based on percentile.
/// </summary>
/// <param name="buckets">Histogram.</param>
/// <param name="numbersCount">Numbers count in histogram.</param>
/// <param name="numbersCountUnderMin">Numbers count under minimum of histogram.</param>
/// <param name="percentile">Percentile.</param>
/// <returns>Bucket.</returns>
HistogramObject findBucket(std::vector<HistogramObject> buckets, size_t numbersCount, size_t numbersCountUnderMin, double percentile)
{
	HistogramObject resultBucket;

	// Calculate position of desired bucket.
	size_t calculatedPosition = (size_t)std::floor(percentile * numbersCount);

	// Add numbers count which are under minimum of histogram to cumulative frequency.
	size_t cumulativeFrequency = numbersCountUnderMin;
	size_t position = 0;

	// If percentil is 100%.
	if (calculatedPosition == numbersCount)
	{
		// Reverse iterate through histogram - we need to find last bucket with frequency -> that is 100% of histogram.
		size_t position = BUCKET_COUNT - 1;
		for (std::vector<HistogramObject>::reverse_iterator it = buckets.rbegin(); it != buckets.rend(); ++it)
		{
			HistogramObject& obj = *it;
			if (obj.getFrequency() > 0)
			{
				resultBucket = obj;
				break;
			}

			position--;
		}
	}
	else
	{
		// Iterate through every bucket.
		for (HistogramObject& bucket : buckets)
		{
			// Add bucket frequency to cumulitave frequency.
			cumulativeFrequency += bucket.getFrequency();

			// Cumulative frequency must be more than calculatedPosition from percentile to actually find result bucket.
			if (cumulativeFrequency > calculatedPosition)
			{
				break;
			}

			position++;
		}

		resultBucket = buckets[position];
	}

	return resultBucket;
}

/// <summary>
/// Method for thread. Same as finding bucket when using single processor, but with less data.
/// </summary>
/// <param name="histogram">Histogram object.</param>
/// <param name="min">Minimum allowed value of histogram.</param>
/// <param name="max">Maximum allowed value of histogram.</param>
void createSubHistogram(HISTOGRAM& histogram, double min, double max)
{
	std::vector<HistogramObject> buckets = createBuckets(min, max);
	size_t numbersCount = 0;
	size_t numbersCountUnderMin = 0;

	while(true) 
	{
		// Get block of data and process it.
		BUFFER_OBJECT buffer;
		bool popped = queue.Pop(buffer);

		// If there is no free block to process and reading file was ended, end thread.
		if (!popped)
		{
			break;
		}
		else
		{
			// Process data block -> find values + count numbers.
			COUNTER_OBJECT counterObj = processDataBlock(buckets, buffer.buffer, buffer.readCount, min, max);

			numbersCount += counterObj.numbersCount;
			numbersCountUnderMin += counterObj.numbersCountUnderMin;

			delete[] buffer.buffer;
		}
	}

	// Assign buckets and counters to histogram struct.
	histogram.buckets = buckets;
	histogram.numbersCount = numbersCount;
	histogram.numbersCountUnderMin = numbersCountUnderMin;
}
