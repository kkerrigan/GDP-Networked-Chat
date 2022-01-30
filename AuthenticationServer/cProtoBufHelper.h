#ifndef _cProtoBuffHelper_HG_
#define _cProtoBuffHelper_HG_

#include <string>

#include "AuthenticationProtocol.pb.h"
#include "../include/cBuffer.h";

class cProtoBufHelper {
public:
	void test() {
		AuthenticateWeb web;
		web.set_email("email");
		web.set_plaintextpassword("password");
	}

	cBuffer createAuthenticateWebMessage(std::string email, std::string password, int socketId);
	cBuffer createAuthenticateWebSuccess(std::string email, int user_id, std::string createdAtDate, int socketId);
	cBuffer createAuthenticateWebFail(AuthenticateWebFailure::eReason reason, int socketId);
	cBuffer createRegisterAccount(std::string email, std::string password, int socketId);
	cBuffer createRegisterAccountSuccess(int user_id, int socketId);
	cBuffer createRegisterAccountFail(CreateAccountWebFailure::eReason user_id, int socketId);

};

#endif
