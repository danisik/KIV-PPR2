#include "FileReader.h"

bool FileReader::readNextBlock(std::vector<char>& data)
{	
	data.clear();

	if (this->stream && this->stream.eof() == false)
	{		
		data.resize(BLOCKSIZE);
		this->stream.seekg(this->offset * BLOCKSIZE);
		this->stream.read(reinterpret_cast<char*>(data.data()), BLOCKSIZE);

		this->offset++;

		data.resize(this->stream.gcount());

		if (this->stream.gcount() == 0)
		{
			return false;
		}
		else 
		{
			return true;
		}		
	}
	else
	{
		return false;
	}
}

void FileReader::closeStream()
{
	this->stream.close();
}

bool FileReader::isStreamOpen()
{
	if (stream && stream.is_open())
	{
		return true;
	}

	return false;
}

FileReader::FileReader(std::string dataFilePath) : stream(dataFilePath, std::ios::in | std::ios::binary)
{
	this->offset = 0;
}
