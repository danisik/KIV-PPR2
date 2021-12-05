#pragma once

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