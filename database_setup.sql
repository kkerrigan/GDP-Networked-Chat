create database IF NOT EXISTS INFO6016_authentication;

use INFO6016_authentication;

/* user
2.1. id BIG INT AUTO_INCREMENT;
2.2. last_login TIMESTAMP;
2.3. creation_date DATETIME;
*/
create table IF NOT EXISTS user(
	id bigint auto_increment,
    last_login timestamp,
    creation_date datetime,
    
    primary key(id)
);

/* web_auth 
1.1. id BIG INT AUTO_INCREMENT;
1.2. userId BIGINT;
1.3. email VARCHAR(255);
1.4. salt CHAR(64);
1.5. hashed_password CHAR(64);
*/
create table IF NOT EXISTS web_auth(
	id bigint auto_increment,
    user_id bigint unique,
    email varchar(255) ,
    salt char(64),
    hashed_password char(64),
    
    primary key(id),
    FOREIGN KEY(user_id) references user(id),
    
    /*
    Index the following as we will be looking up by these columns
    */
    index idx_user_id (user_id), 
    index idx_email (email)
);