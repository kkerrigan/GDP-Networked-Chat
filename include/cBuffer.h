#ifndef _cBuffer_HG_
#define _cBuffer_HG_

#include <stdint.h>
#include <string>
#include <vector>

class cBuffer 
{
public:
	static const unsigned short DEFAULT_BUFFER_LENGTH = 512;

	cBuffer();
	~cBuffer();

	void clear();
	void CheckBufferSize(std::size_t size);
	void loadBuffer(const char* recvbuf, size_t size);
	void appendToBuffer(cBuffer& buffer);
	void flushBuffer();
	const char* toCharArray();

	bool isEmpty();
	std::size_t getLength() const;
	std::size_t getReadIndex() const;
	std::size_t getWriteIndex() const;

	void setReadIndex(std::size_t index);
	void setWriteIndex(std::size_t index);

	void writeChar(char value);
	char readChar();

	void writeStringBE(const std::string& value);
	void writeStringBE(std::size_t index, const std::string& value);
	std::string readStringBE(std::size_t stringLength);
	std::string readStringBE(std::size_t index, std::size_t stringLength);

	void writeUIntBE(std::size_t index, uint32_t value);
	void writeUIntBE(uint32_t value);
	void writeUIntLE(std::size_t index, uint32_t value);
	void writeUIntLE(uint32_t value);

	void writeIntBE(std::size_t index, int32_t value);
	void writeIntBE(int32_t value);
	void writeIntLE(std::size_t index, int32_t value);
	void writeIntLE(int32_t value);

	void writeShortBE(std::size_t index, short value);
	void writeShortBE(short value);
	void writeShortLE(std::size_t index, short value);
	void writeShortLE(short value);

	uint32_t readUIntBE(std::size_t index);
	uint32_t readUIntBE();
	uint32_t readUIntLE(std::size_t index);
	uint32_t readUIntLE();

	int32_t readIntBE(std::size_t index);
	int32_t readIntBE();
	int32_t readIntLE(std::size_t index);
	int32_t readIntLE();

	short readShortBE(std::size_t index);
	short readShortBE();
	short readShortLE(std::size_t index);
	short readShortLE();

private:

	//std::string _bufferString;
	std::vector<uint8_t> _buffer;
	int writeIndex;
	int readIndex;
};


#endif