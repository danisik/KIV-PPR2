#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>

#include "Utils.h"
#include "Histogram.h"

#define BLOCKSIZE sizeof(double)
constexpr const long BUCKET_COUNT = (0.5 * MB) / sizeof(HistogramObject);

int wmain(int argc, wchar_t** argv);
HistogramObject getBucket(std::string filePath, double percentile, std::string cpu, double min, double max);

