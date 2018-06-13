#ifndef _DEF_HPP_
#define _DEF_HPP_

#include <iostream>
#include <list>
#include <string>
#include <iomanip>
#include <vector>
#include <iterator>
#include <map>
#include <assert.h>     /* assert */
#include <algorithm>
using namespace std;

typedef enum SYMBOL_TYPE {ARGUMENT, LIB_FUNC,GLOBAL, FORMAL, LOCAL,LOCAL_IN_FUNC,USERFUNC} SYMBOL_TYPE  ;
typedef enum UNION_TYPE {VARIABLE, FUNCTION} UNION_TYPE;


class Function_Argument{
public
	:
	string 		name;
	int 		offset;
	Function_Argument():name("klodjan "){}
    Function_Argument(string nm):name(nm){}
	    Function_Argument(string nm, int off):name(nm),offset(off){}
	 string getName(){
			return this->name;
	}
    
    ~Function_Argument(){}
};

class Variable {
public
	:
    const string   		name;
    unsigned int        scope;
    unsigned int        line;
	unsigned         offset;
    
	Variable(void){}
    Variable(const string nm, unsigned int scp, unsigned int ln):
    name(nm),scope(scp),line(ln){}
	
 Variable(const string nm, unsigned int scp, unsigned int ln,unsigned  off):
    name(nm),scope(scp),line(ln),offset(off){}

//	const string getName(){
			//cout<< this->name<<"def.hpp 50\n";
//			return this->name;
//	}

	unsigned int getScope(){
		return this->scope;
	}
	unsigned int getLine(){
		return this->line;
	}
	
    ~Variable(void){}
};

class   Function {
public:
    const string       name;
    //list of args
    list<Function_Argument*>args;
    unsigned int            scope;
    unsigned int            line;
    int                     iadress;
    int                     totalLocals;

    Function(void){}
    Function(const string nm,list<Function_Argument*> _args,
             unsigned int scp, unsigned int ln):
             name(nm),args(_args),scope(scp),line(ln){}

    Function(const string nm,list<Function_Argument*> _args,
             unsigned int scp, unsigned int ln,int iadress_,int totL):
             name(nm),args(_args),scope(scp),line(ln),iadress  		      (iadress_),totalLocals(totL){}

	const string getName(){
		return this->name;
	}
	 unsigned int  getScope(){
		return this->scope;
	 }
	 unsigned int  getLine(){
		return this->line;
	 }
	  list<Function_Argument*> &getArguments(){
		return this->args;
	  }
    
    ~Function(void){this->args.clear();}
    
};

class Symbol {
public
	:
	union value {
        Variable*   varVal;
        Function*   funcVal;
    };

    value val;
	bool 				isActive;
	enum SYMBOL_TYPE 	symbolType;
	enum UNION_TYPE 	scopeType;
	int 				line;

		Symbol(SYMBOL_TYPE tp,Function* func):
		symbolType(tp),isActive(true){

				this->val.funcVal=func;
				this->scopeType   = FUNCTION;

			}
		
		Symbol(SYMBOL_TYPE tp, Variable* var, Function* func):
		symbolType(tp),isActive(true)
		{
			
			if(var!=NULL)
			{
				 this->val.varVal    = var;
				this->scopeType     = VARIABLE;

			}
			
			else if(func!=NULL)
			{
				this->val.funcVal=func;
				this->scopeType   = FUNCTION;

			}
			
		}
		 void            Print   (void);
		bool getIsActive(){
			return this->isActive;
		}
		 SYMBOL_TYPE getSymbolType(){
			return this->symbolType;
		}
		
		 UNION_TYPE getScopeType(){
			return this->scopeType;
		}
		int  getLine(){
			return this->line;
		}
		value &getValue(){
			this->val;
		}
		 
		~Symbol(void){}
		
};

void Hide(int scopeToHide);
void scpInsert(unsigned   scope, Symbol* ptr);
bool isLibSymbol(const string str);
Symbol* Insert(Symbol* entry);
Symbol* LookUp(string nameToLook,unsigned scopeToLook);
void PrintSymbolTable();
void initSymbolTable();
#endif
