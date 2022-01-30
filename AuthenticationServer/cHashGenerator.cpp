#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <stdio.h>
#include <random>
#include <string>

#include "cHashGenerator.h";

// c'tor
cHashGenerator::cHashGenerator() {

	SHA256_Init(&this->ctx);
}

// d'tor
cHashGenerator::~cHashGenerator() {}

/*
*	generateHashFromPlainText()
*	This function creates a SHA-256 hash from plain text.
*	Accepts: 
		- A const char pointer representing the plain text.
		- A char array that will contain the hashed password.
*	Returns: None
*/
void cHashGenerator::generateHashFromPlainText(const char* plainText, char* hashBuffer) {

	unsigned char digest[SHA256_DIGEST_LENGTH];

	SHA256_Update(&this->ctx, plainText, strlen(plainText));
	SHA256_Final(digest, &this->ctx);

	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&(hashBuffer[i * 2]), "%02x", (unsigned int)(digest[i]));
	}

	return;

}

/*
*	generateRandomSalt()
*	This function generates a char array of 64 random character that will be used as a salt.
*	Accepts: A char array that will contain the generated salt.
*	Returns: None
*/
void cHashGenerator::generateRandomSalt(char* saltBuffer) {

	// create uniform distribution random device
	std::random_device random;
	std::uniform_int_distribution<int> dist(33, 126); // see http://www.asciitable.com/ for explaination of distribution values

	for (unsigned int i = 0; i != SHA256_DIGEST_LENGTH * 2; ++i) {
		saltBuffer[i] = static_cast<char>(dist(random) & 0xFF);
	}

	saltBuffer[64] = '\0';

	return;
}