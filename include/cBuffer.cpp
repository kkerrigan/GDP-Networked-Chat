#include "cBuffer.h"
#include <stdio.h>

// c'tor
cBuffer::cBuffer() {
	this->_buffer.resize(cBuffer::DEFAULT_BUFFER_LENGTH);
	this->writeIndex = 0;
	this->readIndex = 0;
}

// d'tor
cBuffer::~cBuffer(){}

/*
*	clear()
*	This function resets the read and write index of the buffer and clears the contents.
*	Accepts: None
*	Returns: None
*/
void cBuffer::clear()
{
	for (int i = 0; i < this->getLength(); ++i) {
		this->_buffer[i] = '\0';
	}

	this->writeIndex = 0;
	this->readIndex = 0;
}

/*
*	checkBufferSize()
*	This function checks if the buffer is full and grows it if needed.
*	Accepts: A size_t representing the size to grow the buffer if needed.
*	Returns: None
*/
void cBuffer::CheckBufferSize(std::size_t size)
{
	//Grow buffer if new value will pass current size
	if (size + this->writeIndex > this->_buffer.size())
		this->_buffer.resize(this->_buffer.size() + size);
}

/*
*	loadBuffer()
*	This function fills this buffer from the static buffer used in recieve function.
*	Accepts:
		- A const char pointer representing the recieve buffer.
		- A size_t representing the size of the static buffer.
*	Returns: None
*/
void cBuffer::loadBuffer(const char* recvbuf, size_t size)
{
	//Grow buffer if new value will pass current size
	this->CheckBufferSize(size);

	//Add each byte from the static buffer to class memeber buffer
	for (size_t i = 0; i < size; ++i)
	{
		this->_buffer.at(this->writeIndex) = recvbuf[i];
		this->writeIndex++;
	}
}

/*
*	appendToBuffer()
*	This function adds new data to the buffer that is passed to it.
*	Accepts: A cBuffer object representing the buffer to add new data into.
*	Returns: None
*/
void cBuffer::appendToBuffer(cBuffer& buffer)
{
	const char* data = buffer.toCharArray();
	size_t length = buffer.getLength();
	this->loadBuffer(data, length);
}

/*
*	flushBuffer()
*	This function moves the contents of the buffer so that the data that has already been read
	doesn't get read again.
*	Accepts: None
*	Returns: None
*/
void cBuffer::flushBuffer()
{
	if (this->isEmpty())
	{
		this->writeIndex = 0;
		this->readIndex = 0;
	}
	else
	{
		// shift so unread data is at the front of the buffer
		for (int i = 0; i < this->writeIndex - this->readIndex; ++i)
		{
			this->_buffer.at(i) = this->_buffer[i + this->readIndex];
		}

		// move the write index to the new end and reset the read index
		this->writeIndex -= this->readIndex;
		this->readIndex = 0;
	}
}

/*
*	toCharArray()
*	This function translates buffer to const char* array for the winsock send function.
*	Accepts: None
*	Returns: A const char* representing the buffer as a char array.
*/
const char* cBuffer::toCharArray()
{
	return (const char*)&this->_buffer[this->readIndex];
}

/*
*	isEmpty()
*	This function checks to see if the buffer is empty.
*	Accepts: None
*	Returns: A bool representing whether or not the buffer is empty.
*/
bool cBuffer::isEmpty()
{
	return (this->getLength() == 0);
}

/*
*	getLength()
*	This function returns the current length of the buffer.
*	Accepts: None
*	Returns: A size_t representing the length.
*/
std::size_t cBuffer::getLength() const
{
	return this->writeIndex - this->readIndex;
}

/*
*	getReadIndex()
*	This function returns the current read index of the buffer.
*	Accepts: None
*	Returns: A size_t representing the read index.
*/
std::size_t cBuffer::getReadIndex() const
{
	return this->readIndex;
}

/*
*	getWriteIndex()
*	This function returns the current write index of the buffer.
*	Accepts: None
*	Returns: A size_t representing the write index.
*/
std::size_t cBuffer::getWriteIndex() const
{
	return this->writeIndex;
}

/*
*	setReadIndex()
*	This function sets the read index of the buffer.
*	Accepts: A size_t representing the new read index.
*	Returns: None
*/
void cBuffer::setReadIndex(std::size_t index)
{
	this->readIndex = index;
}

/*
*	setWriteIndex()
*	This function sets the write index of the buffer.
*	Accepts: A size_t representing the new write index.
*	Returns: None
*/
void cBuffer::setWriteIndex(std::size_t index)
{
	this->writeIndex = index;
}

/*
*	writeChar()
*	This function writes a char value into the buffer.
*	Accepts: A char representing the new value.
*	Returns: None
*/
void cBuffer::writeChar(char value)
{
	this->_buffer[this->writeIndex] = value;
	this->writeIndex++;
}

/*
*	readChar()
*	This function reads a char from the buffer.
*	Accepts: None
*	Returns: A char representing the read value.
*/
char cBuffer::readChar()
{
	return this->_buffer[this->readIndex++];
}

/*
*	writeStringBE()
*	This function writes a big endian string to the buffer.
*	Accepts: A string representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeStringBE(const std::string& value)
{
	// call the other write string at the current index
	this->writeStringBE(this->writeIndex, value);
	this->writeIndex += value.size();
}

/*
*	writeStringBE()
*	This function writes a big endian string to the buffer starting at a specific index.
*	Accepts: 
		- A size_t representing where to start writting in the buffer.
		- A string representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeStringBE(std::size_t index, const std::string& value)
{
	for (size_t i = 0; i < value.size(); i++)
	{
		this->_buffer.at(index++) = value.at(i);
	}
}

/*
*	readStringBE()
*	This function reads a big endian string from the buffer.
*	Accepts: A size_t representing the length of the string.
*	Returns: A string representing the value read from the buffer.
*/
std::string cBuffer::readStringBE(std::size_t stringLength)
{
	// call the other read string at the current read index
	std::string  value = this->readStringBE(readIndex, stringLength);
	this->readIndex += value.size();
	return value;
}

/*
*	readStringBE()
*	This function reads a big endian string from the buffer starting at a specific index.
*	Accepts:
		- A size_t representing where to start reading from the buffer.
		- A size_t representing the length of the string.
*	Returns: A string representing the value read from the buffer.
*/
std::string cBuffer::readStringBE(std::size_t index, std::size_t stringLength)
{
	std::string value;

	for (size_t i = 0; i < stringLength; i++)
	{
		char temp = this->_buffer[index++];
		value.append(1, temp);
	}

	return value;
}

/*
*	writeUIntBE()
*	This function writes a big endian unsigned int to the buffer starting at a specific index.
*	Accepts:
		- A size_t representing where to start writting in the buffer.
		- An unsigned int representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeUIntBE(std::size_t index, uint32_t value)
{
	this->_buffer[index++] = value >> 24;
	this->_buffer[index++] = value >> 16;
	this->_buffer[index++] = value >> 8;
	this->_buffer[index++] = value;
}

/*
*	writeUIntBE()
*	This function writes a big endian unsigned int to the buffer.
*	Accepts: An unsigned int representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeUIntBE(uint32_t value)
{
	// call the other write uint at the current index
	this->writeUIntBE(this->writeIndex, value);
	this->writeIndex += 4;
}

/*
*	writeUIntLE()
*	This function writes a little endian unsigned int to the buffer starting at a specific index.
*	Accepts:
		- A size_t representing where to start writting in the buffer.
		- An unsigned int representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeUIntLE(std::size_t index, uint32_t value)
{
	this->_buffer[index++] = value;
	this->_buffer[index++] = value >> 8;
	this->_buffer[index++] = value >> 16;
	this->_buffer[index++] = value >> 24;
}

/*
*	writeUIntLE()
*	This function writes a little endian unsigned int to the buffer.
*	Accepts: An unsigned int representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeUIntLE(uint32_t value)
{
	// call the other little endian write uint at current index
	this->writeUIntLE(this->writeIndex, value);
	this->writeIndex += 4;
}

/*
*	writeIntBE()
*	This function writes a big endian int to the buffer starting at a specific index.
*	Accepts:
		- A size_t representing where to start writting in the buffer.
		- An int representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeIntBE(std::size_t index, int32_t value)
{
	this->_buffer[index++] = value >> 24;
	this->_buffer[index++] = value >> 16;
	this->_buffer[index++] = value >> 8;
	this->_buffer[index++] = value;
}

/*
*	writeIntBE()
*	This function writes a big endian int to the buffer.
*	Accepts: An int representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeIntBE(int32_t value)
{
	// call the other big endian write int at current index
	this->writeIntBE(this->writeIndex, value);
	this->writeIndex += 4;
}

/*
*	writeIntLE()
*	This function writes a little endian int to the buffer starting at a specific index.
*	Accepts:
		- A size_t representing where to start writting in the buffer.
		- An int representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeIntLE(std::size_t index, int32_t value)
{
	this->_buffer[index++] = value;
	this->_buffer[index++] = value >> 8;
	this->_buffer[index++] = value >> 16;
	this->_buffer[index++] = value >> 24;
}

/*
*	writeIntLE()
*	This function writes a little endian int to the buffer.
*	Accepts: An int representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeIntLE(int32_t value)
{
	// call the other little endian write int at current index.
	this->writeIntLE(this->writeIndex, value);
	this->writeIndex += 4;
}

/*
*	writeShortBE()
*	This function writes a big endian short to the buffer starting at a specific index.
*	Accepts:
		- A size_t representing where to start writting in the buffer.
		- A short representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeShortBE(std::size_t index, short value)
{
	this->_buffer[index++] = (uint8_t)(value >> 8);
	this->_buffer[index++] = (uint8_t)(value);
}

/*
*	writeShortBE()
*	This function writes a big endian short to the buffer.
*	Accepts: A short representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeShortBE(short value)
{
	// call the other big endian write short at current index
	this->writeShortBE(this->writeIndex, value);
	this->writeIndex += 2;
}

/*
*	writeShortLE()
*	This function writes a little endian short to the buffer starting at a specific index.
*	Accepts:
		- A size_t representing where to start writting in the buffer.
		- A short representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeShortLE(std::size_t index, short value)
{
	this->_buffer[index++] = (uint8_t)(value);
	this->_buffer[index++] = (uint8_t)(value >> 8);
}

/*
*	writeShortLE()
*	This function writes a little endian short to the buffer.
*	Accepts: A short representing the value to be written to the buffer.
*	Returns: None
*/
void cBuffer::writeShortLE(short value)
{
	// call the other little endian write short at current index
	this->writeShortLE(this->writeIndex, value);
	this->writeIndex += 2;
}

/*
*	readUIntBE()
*	This function reads a big endian unsigned int from the buffer starting at a specific index.
*	Accepts: A size_t representing where to start reading from the buffer.
*	Returns: An unsigned int representing the value read from the buffer.
*/
uint32_t cBuffer::readUIntBE(std::size_t index)
{
	uint32_t value = 0;
	value |= this->_buffer[index++] << 24;
	value |= this->_buffer[index++] << 16;
	value |= this->_buffer[index++] << 8;
	value |= this->_buffer[index++];
	return value;
}

/*
*	readUIntBE()
*	This function reads a big endian unsigned int from the buffer.
*	Accepts: None
*	Returns: An unsigned int representing the value read from the buffer.
*/
uint32_t cBuffer::readUIntBE()
{
	// call other big endian read unit at current index
	uint32_t value = this->readUIntBE(readIndex);
	this->readIndex += 4;
	return value;
}

/*
*	readUIntLE()
*	This function reads a little endian unsigned int from the buffer starting at a specific index.
*	Accepts: A size_t representing where to start reading from the buffer.
*	Returns: An unsigned int representing the value read from the buffer.
*/
uint32_t cBuffer::readUIntLE(std::size_t index)
{
	uint32_t value = 0;
	value |= this->_buffer[index++];
	value |= this->_buffer[index++] << 8;
	value |= this->_buffer[index++] << 16;
	value |= this->_buffer[index++] << 24;
	return value;
}

/*
*	readUIntLE()
*	This function reads a little endian unsigned int from the buffer.
*	Accepts: None
*	Returns: An unsigned int representing the value read from the buffer.
*/
uint32_t cBuffer::readUIntLE()
{
	// call other little endian read uint at current index
	uint32_t value = this->readUIntLE(readIndex);
	this->readIndex += 4;
	return value;
}

/*
*	readIntBE()
*	This function reads a big endian int from the buffer starting at a specific index.
*	Accepts: A size_t representing where to start reading from the buffer.
*	Returns: An int representing the value read from the buffer.
*/
int32_t cBuffer::readIntBE(std::size_t index)
{
	int32_t value = 0;
	value |= this->_buffer[index++] << 24;
	value |= this->_buffer[index++] << 16;
	value |= this->_buffer[index++] << 8;
	value |= this->_buffer[index++];
	return value;
}

/*
*	readIntBE()
*	This function reads a big endian unsigned int from the buffer.
*	Accepts: None
*	Returns: An int representing the value read from the buffer.
*/
int32_t cBuffer::readIntBE()
{
	// call other big endian read int at current index
	int32_t value = this->readIntBE(this->readIndex);
	this->readIndex += 4;
	return value;
}

/*
*	readIntLE()
*	This function reads a little endian int from the buffer starting at a specific index.
*	Accepts: A size_t representing where to start reading from the buffer.
*	Returns: An int representing the value read from the buffer.
*/
int32_t cBuffer::readIntLE(std::size_t index)
{
	int32_t value = 0;
	value |= this->_buffer[index++];
	value |= this->_buffer[index++] << 8;
	value |= this->_buffer[index++] << 16;
	value |= this->_buffer[index++] << 24;
	return value;
}

/*
*	readIntLE()
*	This function reads a little endian int from the buffer starting at a specific index.
*	Accepts: A size_t representing where to start reading from the buffer.
*	Returns: An int representing the value read from the buffer.
*/
int32_t cBuffer::readIntLE()
{
	// call other little endian read int at current index
	int32_t value = this->readIntLE(this->readIndex);
	this->readIndex += 4;
	return value;
}

/*
*	readShortBE()
*	This function reads a big endian short from the buffer starting at a specific index.
*	Accepts: A size_t representing where to start reading from the buffer.
*	Returns: A short representing the value read from the buffer.
*/
short cBuffer::readShortBE(std::size_t index)
{
	short value = 0;
	value |= this->_buffer[index++] << 8;
	value |= this->_buffer[index++];
	return value;
}

/*
*	readShortBE()
*	This function reads a big endian short from the buffer.
*	Accepts: None
*	Returns: A short representing the value read from the buffer.
*/
short cBuffer::readShortBE()
{
	// call other big endian read short at current index
	short value = this->readShortBE(this->readIndex);
	this->readIndex += 2;
	return value;
}

/*
*	readShortLE()
*	This function reads a little endian short from the buffer starting at a specific index.
*	Accepts: A size_t representing where to start reading from the buffer.
*	Returns: A short representing the value read from the buffer.
*/
short cBuffer::readShortLE(std::size_t index)
{
	short value = 0;
	value |= this->_buffer[index++];
	value |= this->_buffer[index++] << 8;
	return value;
}

/*
*	readShortLE()
*	This function reads a little endian short from the buffer.
*	Accepts: None
*	Returns: A short representing the value read from the buffer.
*/
short cBuffer::readShortLE()
{
	// call other little endian read short at current index
	short value = this->readShortLE(this->readIndex);
	this->readIndex += 2;
	return value;
}