#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#define BLOCKSIZE sizeof(double)

#pragma once
class FileReader
{
	public:
		bool readNextBlock(std::vector<char>& data);
		bool isStreamOpen();
		void closeStream();

		FileReader(std::string dataFilePath);

	private:
		std::ifstream stream;
		uint64_t offset;
};

