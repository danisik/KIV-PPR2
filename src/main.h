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

#include "cl.h"

#undef max

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

// Queue for SMP.
CustomQueue queue;

// Methods.
int wmain(int argc, wchar_t** argv);
void reset_histogram(HISTOGRAM& histogram, int64_t min, int64_t max);
void calculate_histogram_values(HISTOGRAM& histogram, int64_t min, int64_t max);
Histogram_Object get_bucket(std::ifstream& stream, HISTOGRAM& histogram, double percentile, int64_t min, int64_t max);
Histogram_Object find_bucket(HISTOGRAM& histogram, double percentile);
Histogram_Object get_bucket_SMP(std::ifstream& stream, HISTOGRAM& histogram, double percentile, int64_t min, int64_t max);
bool get_number_positions(std::ifstream& stream, double desired_value, NUMBER_POSITION& position);
HISTOGRAM create_histogram(int64_t min, int64_t max);
COUNTER_OBJECT process_data_block(HISTOGRAM& histogram, double* buffer, size_t read_count, int64_t min, int64_t max);
void create_sub_histogram(HISTOGRAM& histogram, int64_t min, int64_t max);



