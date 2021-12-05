#pragma once

#include "Histogram_Object.h"
#include "Utils.h"
#include "Queue.h"
#include "Watchdog.h"

// Watchdog thread.
inline CustomWatchdog watchdog(600);

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

	// Invalid OpenCL platform.
	OPENCL_PLAFTORM_NOT_FOUND = 0x03,

	// Invalid OpenCL platform.
	INVALID_OPENCL_PLATFORM = 0x04,

	// No OpenCL device found.
	OPENCL_DEVICE_NOT_FOUND = 0x05
};

/// <summary>
/// Struct representing histogram (used in SMP).
/// </summary>
struct HISTOGRAM
{
	size_t numbers_count = 0;
	size_t numbers_count_under_min = 0;
	std::vector<Histogram_Object> buckets;

	// We want to have uniform distribution of frequencies of values in buckets -> bucket_size = maximal frequency of numbers in single bucket.
	int64_t bucket_size = 0;

	// Offset for bucket index -> we want to have index from 0, so we need to add offset to every single calculation.
	int64_t bucket_index_offset = 0;
};

// Constants.
static constexpr const unsigned int CUSTOM_BYTE = 8;
static constexpr const unsigned int MB = 1024 * 1024;

// Data buffer size.
static constexpr const long BLOCK_SIZE = MB / sizeof(double);

// Number representing how many buckets must be created in single histogram.
static constexpr const long BUCKET_COUNT = MB / sizeof(Histogram_Object);