/*
	Starting point for hash generation was provided by Lukas Gustafson
	https://github.com/lukgus/OpenSSLExample
*/

#ifndef _cHashGenerator_HG_
#define _cHashGenerator_HG_

#include <openssl\conf.h>
#include <openssl\evp.h>
#include <openssl\err.h>
#include <openssl\sha.h>

#define HASH_LENGTH SHA256_DIGEST_LENGTH * 2 + 1
#define SALT_LENGTH SHA256_DIGEST_LENGTH * 2 + 1

class cHashGenerator {
public:
	
	cHashGenerator();
	~cHashGenerator();

	SHA256_CTX ctx;

	void generateHashFromPlainText(const char* plainText, char* hashBuffer);
	void generateRandomSalt(char* saltBuffer);
};

#endif
