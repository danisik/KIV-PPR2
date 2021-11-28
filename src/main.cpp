#include "main.h"

int wmain(int argc, wchar_t** argv) {

	/*
	- Parametry: pprsolver.exe soubor percentil procesor
	soubor - cesta k souboru, mùe bıt relativní k program.exe, ale i absolutní
		percentil - èíslo 1 - 100
		procesor - øetìzec urèujíící, na jakém procesoru a jak vıpoèet probìhne
			single - jednovláknovı vıpoèet na CPU
			SMP - vícevláknovı vıpoèet na CPU
			anebo název OpenCL zaøízení - pozor, v systému mùe bıt nìkolik OpenCL platforem


	- Èísla ze souboru: 64-bitovı double


	- Jako èísla budete uvaovat pouze ty 8-bytové sekvence, pro které std::fpclassify vrátí FP_NORMAL nebo FP_ZERO. 
		Jiné sekvence budete ignorovat. Všechny nuly jsou si stejnì rovné.


	- Pamì bude omezená na 250 MB


	- Program musí skonèit do 15 minut


	- Explicitní upozornìní dle postupnıch dotazù z øad studentù:
		- Program nebude mít povoleno vytváøet soubory na disku.
		- Jako èísla budete uvaovat pouze ty 8-bytové sekvence, pro které std::fpclassify vrátí FP_NORMAL nebo FP_ZERO. 
			Jiné sekvence budete ignorovat. Všechny nuly jsou si stejnì rovné.
		- Pozice v souboru vypisujte v bytech, tj. indexováno od nuly.
		- Hexadecimální èíslo vypište napø. pomocí std::hexfloat.


		TODO: OpenCL CUDA https://medium.com/@pratikone/opencl-on-visual-studio-configuration-tutorial-for-the-confused-3ec1c2b5f0ca
	*/

	// Check if there is 3 arguments.
	if (argc != 4)
	{
		std::wcout << "Invalid number of arguments" << std::endl;
		return 1;
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

	// Processor argument.
	std::wstring processorWS(argv[3]);
	std::string processor;

	std::transform(processorWS.begin(), processorWS.end(), std::back_inserter(processor), [](wchar_t c) {
		return (char)c;
	});

	// To be sure, lower all characters in processor variable.
	std::string processorLower = Utils::toLower(processor);

	// Just for testing purposes.
	filePath = "C:\\Users\\danisik\\Desktop\\PPR\\semestralka\\semestralni_prace\\data.iso";
	percentile = (double)0 / (double)100;
	processor = "smp";	

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
		std::wcout << "file not exists" << std::endl;
		system("pause");
		return 1;
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
		else if (processor == "mpi")
		{
			// TODO: in progress.
			break;
		}
		else
		{
			std::cout << "Invalid processor type - " << processor << std::endl;
			std::cout << "Allowed values: single, SMP, MPI" << std::endl;
			system("pause");
			return 1;
		}

		min = resultBucket.getMinValueFile();
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
	return 0;
}

HistogramObject getBucket(std::ifstream& stream, double percentile, double min, double max)
{
	// Create buffer on heap, because it is too large to save on stack.
	double* buffer = new double[BLOCKSIZE];

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

HistogramObject getBucketSMP(std::ifstream& stream, double percentile, double min, double max)
{
	queue.RestartShutdown();

	// Seek stream to start.
	stream.clear();
	stream.seekg(0);
		
	// Create threads.
	std::vector<std::thread> threads;
	std::vector<HISTOGRAM> threadHistograms;

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
		double* buffer = new double[BLOCKSIZE];

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

bool getNumberPositions(std::ifstream& stream, double desiredValue, NUMBER_POSITION& position)
{
	// Create buffer on heap, because it is too large to save on stack.
	double* buffer = new double[BLOCKSIZE];

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

HistogramObject findBucket(std::vector<HistogramObject> buckets, size_t numbersCount, size_t numbersCountUnderMin, double percentile)
{
	HistogramObject resultBucket;

	// Calculate position of desired bucket.
	size_t calculatedPosition = (size_t)std::floor(percentile * numbersCount);
	size_t cumulativeFrequency = numbersCountUnderMin;
	size_t position = 0;

	// If percentil is 100%.
	if (calculatedPosition == numbersCount)
	{
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
		for (HistogramObject& bucket : buckets)
		{
			cumulativeFrequency += bucket.getFrequency();

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
