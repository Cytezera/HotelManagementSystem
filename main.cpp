#include <iostream>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <thread>
#include <stirng>
using namespace std; 

sql::Connection *con = nullptr; 

sql::Connection* connectDatabase(){
	try{
		sql::mysql::MySQL_Driver *driver; 
		sql::Connection *con; 
		driver = sql::mysql::get_mysql_driver_instance();
		con = driver->connect("tcp//127.0.0.1:3306","root","netacad"); 
		con->setSchema("HotelManagementSystem");
	}catch (sql::SQLException &e){
		cout << "Error Connecting Database" << e.what() << endl; 
		con = nullptr; 
	}
}
int main() {
	connectDatabase(); 
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
				return 0 ;
			default:
				cout << "Invalid Input" << endl; 	
		}	
	}
}
