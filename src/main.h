#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>

#include "Utils.h"
#include "Histogram.h"

static constexpr const unsigned int MB = 1024 * 1024;
static constexpr const double DOUBLE_MIN = std::numeric_limits<double>::lowest();
static constexpr const double DOUBLE_MAX = std::numeric_limits<double>::max();

static constexpr const long BLOCKSIZE = MB / sizeof(double);
static constexpr const long BUCKET_COUNT = MB / sizeof(HistogramObject);

double buffer[BLOCKSIZE];

int wmain(int argc, wchar_t** argv);
HistogramObject getBucket(std::string filePath, double percentile, std::string cpu, double min, double max);

