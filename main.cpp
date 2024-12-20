
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
string logInUsername; 

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
	string checkInDate , checkOutDate; 
	cout << "Please inert the date you would like to check in : " << endl ;
	cin >> checkInDate ;
	cout << "Please inesert the date you would like to check out : " << endl; 
	cin >> checkOutDate; 
	string	query =" select r.roomID, r.room_type "
	       	"from rooms r "
	       	"left join book b on r.roomID = b.roomID "
	       	"AND ((b.check_in_date <= ? and b.check_out_date >= ? ) "
		"OR (b.check_in_date <= ? and b.check_out_date >= ?) ) "
	      	"where b.book_id is null;";	
	try{
		sql::PreparedStatement* pstmt = con->prepareStatement(query); 
		pstmt->setString(1,checkInDate);
		pstmt->setString(2,checkInDate); 
		pstmt->setString(3,checkOutDate);
		pstmt->setString(4,checkOutDate); 
		sql::ResultSet* res = pstmt->executeQuery();	
		cout << "Rooms available during these dates " << endl; 
		while (res -> next()){

			int roomID = res->getInt("roomID");
			string roomType = res->getString("room_type");
			cout << "Room ID: " << roomID << "\nRoom Type: " << roomType << endl; 
			cout << "\n"; 
		
		}
		delete pstmt ;
		delete res; 
	}catch (sql::SQLException& e) {
		cerr << "Error getting rooms aviableble" << e.what() << endl; 
	}
}
void bookRoom(){
	int roomID; 
	string checkInDate , checkOutDate ; 
	string query = "insert into book (username, roomID, check_in_date , check_out_date, status ) values (? , ? , ? , ? , 'unpaid'); " ; 
	cout << "Which room you would like to check in? (room_id) : " << endl; 
	cin >> roomID ; 
	cout << "When would you like to check in ? " << endl; 
	cin >> checkInDate ; 
	cout << "When would you like to check out? " << endl; 
	cin >> checkOutDate ; 
	try{
		sql::PreparedStatement* pstmt = con->prepareStatement(query); 
		pstmt->setString(1,logInUsername); 
		pstmt->setInt(2, roomID) ;
		pstmt ->setString (3, checkInDate) ; 
		pstmt ->setString (4, checkOutDate) ; 
		pstmt->executeUpdate(); 
	}
	catch ( sql::SQLException& e) {
		cerr << "Error in booking room " << e.what() << endl; 
	}
}
void generateInvoice(){
	int roomId, days ; 
	double cost; 
	string query = "select roomID , DATEDIFF(check_out_date, check_in_date) as days, cost from book where username = ? and status = 'unpaid'; " ; 
       	sql::PreparedStatement* pstmt = con->prepareStatement(query); 
	pstmt->setString(1,logInUsername); 
	sql::ResultSet* res = pstmt->executeQuery(); 
	while (res -> next()) {
		roomId = res->getInt("roomID"); 
		days = res->getInt("days") ; 
		cost = res->getDouble("cost") ; 
		cout << "Room ID: " << roomId << endl; 
		cout << "Number of nights " << days << endl; 
		cout << "Amount: " << cost << "\n " << endl; 

	}	
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
	string query  = "select * from users where username = ? ; " ;
	string username, hashedPassword, actualPassword, password; 
	sql::ResultSet* res; 
	while (true){
		cout << "Please insert your username: " << endl; 
	       	cin >> username; 
		sql::PreparedStatement* pstmt = con->prepareStatement(query); 
		pstmt->setString(1,username); 
		res = pstmt->executeQuery();  
		if (res -> next()){
			logInUsername = res->getString("username"); 
			break ; 
		}else {
			cout << "Invalid username " << endl; 
		}	
				
	}
	actualPassword = res->getString("password"); 
	while (true) {

		cout << "Please insert your password: "  << endl; 
		cin >> password; 
		hashedPassword = hashPassword(password) ;
		if (actualPassword == hashedPassword){
			cout << "Log in successful" << endl; 
			cout << "Hello " << logInUsername << endl ;   
			menu();
			return; 
		}
		cout << "Wrong pasword, please try again "  << endl; 
	}
}
void landingPage(){

	int userInput; 
	while (true){
		cout << "1. Register new account " << endl ;
		cout << "2. Log in " << endl; 
		cout << "3. Exit " << endl ;
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
				cout << "Turning off " << endl ;
				return  ; 
				
			default:
				cout << "Invalid input " << endl; 
		}
	}
}
int main() {
	connectDatabase(); 
	landingPage(); 
}
