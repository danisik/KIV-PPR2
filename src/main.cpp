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
	
	std::string filePath = "C:\\Users\\danisik\\Desktop\\PPR\\semestralka\\semestralni_prace\\data.iso";
	double percentile = (double)55 / (double)100;
	std::string cpu = "";

	double min = DOUBLE_MIN;
	double max = DOUBLE_MAX;
	HistogramObject resultBucket;

	auto t_start = std::chrono::high_resolution_clock::now();
	int test = 0;

	// Return buckets until only single number is presented.
	while (true)
	{
		resultBucket = getBucket(filePath, percentile, cpu, min, max);
		min = resultBucket.getMin();
		max = resultBucket.getMax();

		test++;				
		if (test == 1)
		{
			std::wcout << std::endl;
			std::wcout << "Min: " << resultBucket.getMin() << std::endl;
			std::wcout << "Max: " << resultBucket.getMax() << std::endl;
			std::wcout << "Frequency: " << resultBucket.getFrequency() << std::endl;
 			break;
		}		

		if (min == max)
		{
			break;
		}
	}

	// TODO: Iterate through file to find first and last position of number.
	// *

	auto t_end = std::chrono::high_resolution_clock::now();
	double elapsed_time = std::chrono::duration<double, std::milli>(t_end - t_start).count() / 1000;
	std::wcout << "Time: " << elapsed_time << " sec" << std::endl;

	system("pause");
}

HistogramObject getBucket(std::string filePath, double percentile, std::string cpu, double min, double max)
{
	// Init return vals.
	HistogramObject resultBucket;

	std::ifstream stream(filePath, std::ios::in | std::ios::binary);

	if (!(stream && stream.is_open()))
	{
		// An error occurred!
		// myFile.gcount() returns the number of bytes read.
		// calling myFile.clear() will reset the stream state
		// so it is usable again.
		std::wcout << "file not exists" << std::endl;
	}
	else
	{
		std::vector<HistogramObject> buckets;		

		long i = 0;
		long numbersCount = 0;		

		// Calculate step.
		double step = std::abs(min / BUCKET_COUNT) + (max / BUCKET_COUNT);
		double from = min;
		double to = min + step;

		// Create buckets and set range.
		for (long i = 0; i < BUCKET_COUNT; i++)
		{
			if (i == BUCKET_COUNT - 1) to = max;
			HistogramObject object(from, to);
			buckets.push_back(object);

			from = to;
			to += step;
		}

		uint64_t offset = 0;
		// Read file by 8byte blocks.
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

			long read = stream.gcount() / sizeof(double);

			for (long i = 0; i < read; i++)
			{
				double value = buffer[i];
				// Check if double value is correct value.
				if (Utils::isCorrectValue(value))
				{
					// Check if value is in histogram range.
					if (value <= max && value >= min)
					{
						// Get position using binary search.
						//long position = 1;
						long position = Utils::binarySearch(buckets, 0, buckets.size() - 1, value);

						// If position was found.
						if (position != -1)
						{
							HistogramObject& bucket = buckets[position];
							bucket.incrementFrequency();
						}
						else
						{
							buckets.back().incrementFrequency();
						}

						// Increment numbers.
						numbersCount++;
					}
				}
			}
		}

		// Calculate position of desired bucket.
		double calculatedPosition = std::ceil(percentile * numbersCount);
		long cumulativeFrequency = 0;
		long position = 0;
		for (HistogramObject& bucket : buckets)
		{
			cumulativeFrequency += bucket.getFrequency();

			if (cumulativeFrequency >= calculatedPosition)
			{
				break;
			}

			position++;
		}

		resultBucket = buckets[position];
	}

	stream.close();

	return resultBucket;
}