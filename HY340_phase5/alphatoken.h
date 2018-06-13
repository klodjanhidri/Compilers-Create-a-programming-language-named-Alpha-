	#include   "parser.hpp"
 	#include    <stdlib.h>
	#include    <string.h>
	#include    <fstream>

    #include <iostream>
  	#include <vector>
	#include <string>
  //#include <iomanip>

	using namespace std;
	 int openc , closec,closes=0,opens=0,tokenCounter=0,tmpLine=0,strError=0,foundErrors =0;
	int i=1,slash,ilegal,k,tmp1;
	string str;
	 bool            stringClosed    = false;
           
	class alpha_token{
		private:
			unsigned token_counter;
			unsigned line_counter;
			string	token_type;
			string	token_name;
			string data ;
		public:
		alpha_token(){}
		alpha_token(unsigned token_count,unsigned line_count,string	token_t,
			string	token_n,string d):
			token_counter(token_count),
			line_counter(line_count),
			token_type(token_t),
			token_name(token_n),
			data(d){}
		~alpha_token(){}

		int gettk(){
			return token_counter;
		}
		int getlc(){
			return line_counter;
		}
		string gettp(){
			return token_type;
		}
		string gettn(){
			return token_name;
		}
		string getdata(){
			return data;
		}
		
		};
		int j,c;
		int line_num = 1;
		int tokenNumber=1;
		vector<int>nestedline;
	typedef	vector<alpha_token>alpha_token_t;
	alpha_token_t tokens_information;
	
        int open_comment,close_comment,tmp;
	int str_line;
        bool string_error,illegal=false ;
        string buffer_tmp;
