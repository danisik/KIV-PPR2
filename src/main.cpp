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
	*/
	std::string filePath = "C:\\Users\\danisik\\Desktop\\PPR\\semestralka\\semestralni_prace\\party.mp3";
	double percentile = (double)61 / (double)100;
	std::string cpu = "";

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
		resultBucket = getBucket(stream, percentile, cpu, min, max);
		min = resultBucket.getMinValueFile();
		max = resultBucket.getMaxValueFile();

		cycles++;

		// If bucket is represented as single number, then we found what we want.
		if (min == max)
		{
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

	// TODO: Iterate through file to find first and last position of number.
	// *
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

HistogramObject getBucket(std::ifstream& stream, double percentile, std::string cpu, double min, double max)
{
	// Seek stream to start.
	stream.clear();
	stream.seekg(0);

	// Init return vals.
	HistogramObject resultBucket;
	std::vector<HistogramObject> buckets;		

	size_t i = 0;
	size_t numbersCount = 0;
	size_t numbersCountUnderMin = 0;

	// Calculate step.
	double step = std::abs(min / BUCKET_COUNT) + (max / BUCKET_COUNT);
	double stepp1 = std::abs(min / BUCKET_COUNT);
	double stepp2 = std::abs(max / BUCKET_COUNT);

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
				// Check if value is in histogram range.
				if (value <= max && value >= min)
				{
					// Fix for last bucket -> if value is = max, then it means it must be in the next bucket, because 
					// ´we want <min, max) and we will throw away values = max when applying for last bucket.
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
					numbersCountUnderMin++;
				}

				// Increment total counts.
				numbersCount++;
			}
		}
	}

	// Calculate position of desired bucket.
	size_t calculatedPosition = std::floor(percentile * numbersCount);
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

bool getNumberPositions(std::ifstream& stream, double desiredValue, NUMBER_POSITION& position)
{
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
						// We want position in Bytes.
						position.firstOccurence = counter * BYTE;
						firstOccurenceChanged = true;
					}

					if (position.lastOccurence <= counter)
					{
						// We want position in Bytes.
						position.lastOccurence = counter * BYTE;
						lastOccurenceChanged = true;
					}
				}
			}

			counter++;
		}
	}

	// If occurences were changed, return true.
	if (firstOccurenceChanged && lastOccurenceChanged)
		return true;

	return false;
}