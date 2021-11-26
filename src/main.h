#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

#include "Utils.h"

static constexpr const unsigned int MB = 1024 * 1024;

static constexpr const long BLOCKSIZE = MB / sizeof(double);
static constexpr const long BUCKET_COUNT =  MB / sizeof(HistogramObject);

double buffer[BLOCKSIZE];

int wmain(int argc, wchar_t** argv);
HistogramObject getBucket(std::ifstream& stream, double percentile, std::string cpu, double min, double max);

