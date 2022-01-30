#ifndef DATABASE_HELPER_HG
#define DATABASE_HELPER_HG

#include <cppconn\driver.h>
#include <cppconn\exception.h>
#include <cppconn\resultset.h>
#include <cppconn\statement.h>
#include <cppconn\prepared_statement.h>

#include <string>

class cDatabaseHelper
{
public:
	cDatabaseHelper();
	~cDatabaseHelper();

	bool ConnectToDatabase(const std::string &server, const std::string &username, const std::string &password, const std::string &schema);

	bool Execute(const std::string &message);
	sql::ResultSet* ExecuteQuery(const std::string &message);
	int ExecuteUpdate(const std::string &message);

	bool isUserExist(std::string email);
	bool createUser(std::string email, std::string salt, std::string hashedPassword);
	bool updateLastLogin(std::string email);
	bool verifyPassword(std::string email, std::string password);
	bool getUserIdWithCreateDate(std::string email, int* user_id, std::string* createDate);

private:
	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;
};

#endif
