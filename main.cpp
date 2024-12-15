
#include <iostream>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <thread>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <string>
using namespace std; 

sql::Connection *con = nullptr; 

string hashPassword(string password){
	unsigned char hash[SHA256_DIGEST_LENGTH]; 
	EVP_MD_CTX* ctx = EVP_MD_CTX_new(); 
	EVP_DigestInit_ex(ctx, EVP_sha256(),nullptr);
	EVP_DigestUpdate(ctx, password.c_str() , password.size()); 
	EVP_DigestFinal_ex(ctx, hash, nullptr); 
	EVP_MD_CTX_free(ctx);

	string hashedPassword;
	for (int i = 0 ; i < SHA256_DIGEST_LENGTH; i++){
		char buf[3];
		sprintf(buf,"%02x",hash[i]); 

		hashedPassword.append(buf);

	
	
	}
	return hashedPassword; 
}
void connectDatabase(){
	try{
		sql::mysql::MySQL_Driver *driver; 
		driver = sql::mysql::get_mysql_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306","root","netacad");
		con->setSchema("HotelManagementSystem"); 

	}catch(sql::SQLException &e) {
		cout << "Error Connecting Database" << e.what() << endl; 
		con = nullptr; 
	}
}
void viewRoom(){
}
void bookRoom(){
}
void generateInvoice(){
}
void menu(){
	int userInput; 
	while (true){
		cout << "HOTEL MANAGEMENT SYSTEM" << endl; 
		cout << "1. View Available Room" << endl; 
		cout << "2. Book a room" << endl; 
	       	cout << "3. Generate Invoice" << endl ;
		cout << "4. Exit" << endl; 
		cout << "What would you like to do: " ; 
		cin >> userInput; 
		
		switch(userInput){
			case 1:
				viewRoom(); 
				break; 
			case 2: 
				bookRoom(); 
				break; 
			case 3: 
				generateInvoice();
				break; 
			case 4: 
				cout << "Goodbye .... " << endl; 
				return  ;
			default:
				cout << "Invalid Input" << endl; 	
		}	
	}
}
void registerAccount(){
	string username, query, name, dob, password ,confirmPassword; 
	while (true) {
		cout << "Please select a username" << endl; 
		cin >> username; 
		query = ("select * from users where username = ? " ) ; 
		sql::PreparedStatement* pstmt = con->prepareStatement(query); 
		pstmt->setString(1,username); 
		sql::ResultSet* res = pstmt->executeQuery(); 
		if(res -> next()){
			cout << "Username already exist, try another username " << endl; 
		}
		break; 	
		
	}
	do {
		cout << "Please insert your password" << endl; 
		cin >> password; 
		cout << "Please retype your password" << endl ;
		cin >> confirmPassword; 
		if (password != confirmPassword){
			cout << "Password does not match" << endl; 

		}
	}while(password != confirmPassword); 
	cout << "Please insert your name " << endl; 
	cin >> name; 
	cout << "Insert date of birht (YYYY/MM/DD) " << endl; 
	cin >> dob ; 
	string hashedPassword = hashPassword(password); 
	string query2 = "Insert into users (username,password,name,dob) VALUES (?,?,?,?);" ; 
	sql::PreparedStatement* pstmt2 = con->prepareStatement(query2); 
	pstmt2->setString(1,username);
	pstmt2->setString(2,hashedPassword); 
	pstmt2->setString(3,name); 
	pstmt2->setString(4,dob); 	
	pstmt2->executeUpdate();
	cout << "User created " << endl; 
}
void logIn(){

}
void landingPage(){

	int userInput; 
	while (true){
		cout << "1. Register new account " << endl ;
		cout << "2. Log in " << endl; 
		cout << "Your choice : " ; 
		cin >> userInput; 
		switch(userInput){
			case 1: 
				registerAccount(); 
				break; 
			case 2: 
				logIn(); 
				break; 
			case 3: 
				return  ; 
				
			default:
				cout << "Invalid input " << endl; 
		}
	}
}
int main() {
	connectDatabase(); 
	landingPage(); 
	menu(); 
}
