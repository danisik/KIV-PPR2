#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <thread>
#include <memory>

#include "Utils.h"
#include "Queue.h"

/// <summary>
/// Enum for return codes.
/// </summary>
enum EXIT_CODE : size_t
{
	// Everythins is ok.
    SUCCESS = 0x00,	

	// Invalid arguments provided
    INVALID_ARGS = 0x01,

	// Invalid file.
	INVALID_FILE = 0x02
};

// Structs.

/// <summary>
/// Struct representing first and last position of number.
/// </summary>
struct NUMBER_POSITION 
{
	size_t first_occurence = 0;
	size_t last_occurence = 0;
};

/// <summary>
/// Struct representing numbers count in histogram (+ numbers count under minimum value).
/// </summary>
struct COUNTER_OBJECT
{
	size_t numbers_count = 0;
	size_t numbers_count_under_min = 0;
};

/// <summary>
/// Struct representing histogram (used in SMP).
/// </summary>
struct HISTOGRAM
{
	size_t numbers_count = 0;
	size_t numbers_count_under_min = 0;
	std::vector<Histogram_Object> buckets;
};

// Constants.

// Number representing how many threads are created.
static constexpr const unsigned int THREADS_COUNT = 10;

// Max blocks to be available in queue.
static constexpr const unsigned int MEMORY_BLOCKS_ALLOWED = 2 * THREADS_COUNT;

static constexpr const unsigned int BYTE = 8;
static constexpr const unsigned int MB = 1024 * 1024;

// Data buffer size.
static constexpr const long BLOCK_SIZE = 1 * MB / sizeof(double);

// Number representing how many buckets must be created in single histogram.
static constexpr const long BUCKET_COUNT = 0.6 * MB / sizeof(Histogram_Object);

// Queue for SMP.
Queue queue;

// Methods.
int wmain(int argc, wchar_t** argv);
Histogram_Object get_bucket(std::ifstream& stream, double percentile, double min, double max);
Histogram_Object find_bucket(std::vector<Histogram_Object> buckets, size_t numbers_count, size_t numbers_count_under_min, double percentile);
Histogram_Object get_bucket_SMP(std::ifstream& stream, double percentile, double min, double max);
bool get_number_positions(std::ifstream& stream, double desired_value, NUMBER_POSITION& position);
std::vector<Histogram_Object> create_buckets(double min, double max);
COUNTER_OBJECT process_data_block(std::vector<Histogram_Object>& buckets, double* buffer, size_t read_count, double min, double max);
void create_sub_histogram(HISTOGRAM& histogram, double min, double max);




