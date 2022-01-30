#include <iostream>

#include "cDatabaseHelper.h"
#include "cHashGenerator.h";

// c'tor
cDatabaseHelper::cDatabaseHelper(){}

// d'tor
cDatabaseHelper::~cDatabaseHelper(){}

/*
*	ConnectToDatabase()
*	This function connects to the MySQL database.
*	Accepts:
*		- A string representing the server name.
*		- A string representing the username.
*		- A string representing the password.
*		- A string representing the schema.
*	Returns: A bool representing whether the connection was success.
*/
bool cDatabaseHelper::ConnectToDatabase(const std::string &server, const std::string &username, const std::string &password, const std::string &schema) {
	try {
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		con->setSchema(schema);
	}
	catch (sql::SQLException &exception) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;
		return false;
	}

	return true;
}

/*
*	Execute()
*	This function executes a Prepared Statement.
*	Accepts: A string representing the prepared statement.
*	Returns: A bool representing whether the connection was success.
*/
bool cDatabaseHelper::Execute(const std::string &sql) {
	try {
		pstmt = con->prepareStatement(sql);
		return pstmt->execute();
	}
	catch (sql::SQLException &exception) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;
		return false;
	}
}

/*
*	ExecuteQuery()
*	This function executes a SQL prepared statment.
*	Accepts: A string representing the prepared statement.
*	Returns: A ResultSet containing the results of the query.
*/
sql::ResultSet* cDatabaseHelper::ExecuteQuery(const std::string &sql) {
	try {
		pstmt = con->prepareStatement(sql);
		return pstmt->executeQuery();
	}
	catch (sql::SQLException &exception) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;
		return false;
	}
}

/**
 * ExecuteUpdate
 * Executes the sql command from the string
 * @param message - the sql command
 * @return int - the number of rows affected
 */
int cDatabaseHelper::ExecuteUpdate(const std::string &sql) {
	try {
		pstmt = con->prepareStatement(sql);
		return pstmt->executeUpdate();
	}
	catch (sql::SQLException &exception) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;
		return false;
	}
}

/*
*	isUserExist()
*	This function executes a SQL prepared statement that checks whether a user in the database.
*	Accepts: A string representing the email for the user.
*	Returns: A bool representing if the query was successful.
*/
bool cDatabaseHelper::isUserExist(std::string email) {
	try {
		this->pstmt = this->con->prepareStatement("select count(*) from web_auth where email = ?");
		this->pstmt->setString(1, email);

		sql::ResultSet* result = this->pstmt->executeQuery();

		while (result->next()) {
			return result->getInt(1) > 0;
		}

		// if there is no results assume an error and break the workflow
		return true;
	}
	catch (sql::SQLException &exception) {
		// something bad happened revert the changes we made
		this->con->rollback();

		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;

		// if there is an error assume the user exists to break the workflow
		return true;
	}	
}

/*
*	createUser()
*	This function executes a SQL prepared statement that adds a new user to the database.
*	Accepts: 
		- A string representing the email for the user.
		- A string representing the salt associated with the user.
		- A string representing the hashed password for the user.
*	Returns: A bool representing if the query was successful.
*/
bool cDatabaseHelper::createUser(std::string email, std::string salt, std::string hashedPassword) {
	try {
		if (this->isUserExist(email)) return false;

		// start a transaction
		this->con->setAutoCommit(false);
		 
		this->pstmt = this->con->prepareStatement("insert into user(creation_date) VALUES (now())");
		sql::ResultSet* userResult = this->pstmt->executeQuery();

		if (userResult->next()) {
			if (userResult->getInt(1) != 1) {
				//the user was not inserted
				this->con->rollback();
				return false;
			}
		}

		//get the id so we can add it to the other table
		this->pstmt = this->con->prepareStatement("SELECT LAST_INSERT_ID()");
		sql::ResultSet* lastInsertIdResult = this->pstmt->executeQuery();
		lastInsertIdResult->next();
		int user_id = lastInsertIdResult->getInt(1);

		//insert the other data into the web_auth table
		this->pstmt = this->con->prepareStatement("insert into web_auth(user_id, email, salt, hashed_password) VALUES(?,?,?,?)");
		this->pstmt->setInt(1, user_id);
		this->pstmt->setString(2, email);
		this->pstmt->setString(3, salt);
		this->pstmt->setString(4, hashedPassword);

		sql::ResultSet* webAuthResult = this->pstmt->executeQuery();
		if (webAuthResult->next()) {
			if (webAuthResult->getInt(1) != 1) {
				//the user was not inserted
				this->con->rollback();
				return false;
			}
		}

		// everything ok commit changes
		this->con->commit();

		return true;
	}
	catch (sql::SQLException &exception) {
		// something bad happened revert the changes we made
		this->con->rollback();

		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;

		return false;
	}
}

/*
*	updateLastLogin()
*	This function executes a SQL prepared statement that updates the users last login time.
*	Accepts: A string representing the email for the user.
*	Returns: A bool representing if the query was successful.
*/
bool cDatabaseHelper::updateLastLogin(std::string email) {
	try {
		this->pstmt = this->con->prepareStatement("select user_id from web_auth where email = ?");
		this->pstmt->setString(1, email);

		sql::ResultSet* userResult = this->pstmt->executeQuery();
		
		int user_id = -1;
		while (userResult->next()) {
			user_id = userResult->getInt64(1);
		}
		
		// if the user id cannot be got fail
		if (user_id == -1) {
			return false;
		}

		this->pstmt = this->con->prepareStatement("update user set last_login = now() where id = ?");
		this->pstmt->setInt(1, user_id);

		int rowsModified = this->pstmt->executeUpdate();

		return rowsModified == 1;
	}
	catch (sql::SQLException &exception) {
		// something bad happened revert the changes we made
		this->con->rollback();

		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;

		return false;
	}
}

/*
*	verifyPassword()
*	This function executes a SQL prepared statement that grabs the password information for the user for verification.
*	Accepts: 
		- A string representing the email for the user.
		- A string representing the plain text password entered by the user.
*	Returns: A bool representing if the query was successful.
*/
bool cDatabaseHelper::verifyPassword(std::string email, std::string password) {
	try {
		this->pstmt = this->con->prepareStatement("select salt, hashed_password from web_auth where email = ?");
		this->pstmt->setString(1, email);

		sql::ResultSet* userResult = this->pstmt->executeQuery();


		std::string salt;
		std::string hashedPassword;
		while (userResult->next()) {
			salt = userResult->getString(1);
			hashedPassword = userResult->getString(2);
		}

		password = salt + password;
		char hashArray[SHA256_DIGEST_LENGTH * 2 + 1];

		// Call hash generator
		cHashGenerator generator;
		generator.generateHashFromPlainText(password.c_str(), hashArray);
		std::string hash = std::string(hashArray);

		//verify password here
		if (std::strcmp(hashedPassword.c_str(), hash.c_str()) == 0) {
			return true;
		}
		
		return false;
	}
	catch (sql::SQLException &exception) {
		// something bad happened revert the changes we made
		this->con->rollback();

		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;

		return false;
	}
}

/*
*	getUserIdWithCreateDate()
*	This function executes a SQL prepared statement that retrieves the user ID and create date for the user.
*	Accepts: 
		- A string representing the email for the user.
		- An int pointer that will contain the ID for the user.
		- A string pointer that will contain the created date for the user.
*	Returns: A bool representing if the query was successful.
*/
bool cDatabaseHelper::getUserIdWithCreateDate(std::string email, int* user_id, std::string* createDate){
	try {
		this->pstmt = this->con->prepareStatement("select user_id, creation_date from web_auth inner join user on user.id=web_auth.user_id where email = ?");
		this->pstmt->setString(1, email);

		sql::ResultSet* result = this->pstmt->executeQuery();

		while (result->next()) {
			*user_id = result->getInt(1);
			*createDate = result->getString(2);
		}

		// if there is no results assume an error and break the workflow
		return true;
	}
	catch (sql::SQLException &exception) {
		// something bad happened revert the changes we made
		this->con->rollback();

		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;

		// if there is an error assume the user exists to break the workflow
		return false;
	}
}