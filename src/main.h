#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <thread>

#include "Utils.h"
#include "Queue.h"

// Structs.
struct NUMBER_POSITION 
{
	size_t firstOccurence = 0;
	size_t lastOccurence = 0;
};

struct COUNTER_OBJECT
{
	size_t numbersCount = 0;
	size_t numbersCountUnderMin = 0;
};

struct HISTOGRAM
{
	size_t numbersCount = 0;
	size_t numbersCountUnderMin = 0;
	std::vector<HistogramObject> buckets;
};

// Constants.
static constexpr const unsigned int THREADS_COUNT = 10;
static constexpr const unsigned int MEMORY_BLOCKS_ALLOWED = 2 * THREADS_COUNT;

static constexpr const unsigned int BYTE = 8;
static constexpr const unsigned int MB = 1024 * 1024;

static constexpr const long BLOCKSIZE = MB / sizeof(double);
static constexpr const long BUCKET_COUNT = MB / sizeof(HistogramObject);

// Queue for SMP.
Queue queue;

// Methods.
int wmain(int argc, wchar_t** argv);
HistogramObject getBucket(std::ifstream& stream, double percentile, double min, double max);
HistogramObject findBucket(std::vector<HistogramObject> buckets, size_t numbersCount, size_t numbersCountUnderMin, double percentile);
HistogramObject getBucketSMP(std::ifstream& stream, double percentile, double min, double max);
bool getNumberPositions(std::ifstream& stream, double desiredValue, NUMBER_POSITION& position);
std::vector<HistogramObject> createBuckets(double min, double max);
COUNTER_OBJECT processDataBlock(std::vector<HistogramObject>& buckets, double* buffer, size_t readCount, double min, double max);
void createSubHistogram(HISTOGRAM& histogram, double min, double max);




