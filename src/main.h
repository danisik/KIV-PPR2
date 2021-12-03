#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <thread>
#include <memory>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <sstream>

#include "Utils.h"
#include "Queue.h"
#include "cl.h"

#undef max

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
	INVALID_FILE = 0x02,

	// Calculation cycled.
	CYCLED_CALCULATION = 0x03
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

static constexpr const unsigned int CUSTOM_BYTE = 8;
static constexpr const unsigned int MB = 1024 * 1024;

// Data buffer size.
static constexpr const long BLOCK_SIZE = MB / sizeof(double);

// Number representing how many buckets must be created in single histogram.
static constexpr const long BUCKET_COUNT = MB / sizeof(Histogram_Object);

// Queue for SMP.
CustomQueue queue;

// Watchdog thread.
CustomWatchdog watchdog(111600);

// Methods.
int wmain(int argc, wchar_t** argv);
void reset_buckets(std::vector<Histogram_Object>& buckets, double min, double max);
Histogram_Object get_bucket(std::ifstream& stream, std::vector<Histogram_Object>& buckets, double percentile, double min, double max);
Histogram_Object find_bucket(std::vector<Histogram_Object> buckets, size_t numbers_count, size_t numbers_count_under_min, double percentile);
Histogram_Object get_bucket_SMP(std::ifstream& stream, std::vector<Histogram_Object>& buckets, double percentile, double min, double max);
bool get_number_positions(std::ifstream& stream, double desired_value, NUMBER_POSITION& position);
std::vector<Histogram_Object> create_buckets(double min, double max);
COUNTER_OBJECT process_data_block(std::vector<Histogram_Object>& buckets, double* buffer, size_t read_count, double min, double max);
void create_sub_histogram(HISTOGRAM& histogram, double min, double max);



