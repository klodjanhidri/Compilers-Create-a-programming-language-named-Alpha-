#include "def.hpp"
//============================


typedef multimap<string,Symbol*> BucketList;BucketList sTable;
typedef vector<Symbol*>ScopeList;
typedef vector<ScopeList>ScopeVector;ScopeVector scpVec(1);
typedef vector<Symbol*> ll;ll lib_func;//lista gia thn apothikeush twn sunartisewn bibliothikis 
typedef list<Function_Argument*> arg12;
arg12 ss2;





bool isLibSymbol(string name){
        for(int i=0;i<lib_func.size();i++){
			if ((lib_func[i]->val.funcVal->getName() == name)) {
				return true;
			}
        }
        
	return false;
}

Symbol* Insert(Symbol* entry){
//	assert(entry);
	if(entry->getValue().varVal!=NULL)
        sTable.insert(make_pair<string,Symbol*>(entry->getValue().varVal->name,entry));
    else
        sTable.insert(pair<string,Symbol*>(entry->getValue().funcVal->getName(),entry) );

	return entry;
}

Symbol*   LookUp (string nameToLook, unsigned scopeToLook){
		Symbol  *it=NULL;
		it=sTable.find(nameToLook)->second;
		
		if(it==NULL)
			return NULL;
			
        if(((*it).getSymbolType()==GLOBAL)||((*it).getSymbolType()==LOCAL)||
          ((*it).getSymbolType()==FORMAL)||((*it).getSymbolType()==LOCAL_IN_FUNC))
        {
            if(((*it).getIsActive()==true) &&(*it).val.varVal->getScope()==scopeToLook&&
              (*it).val.varVal->name.compare(nameToLook) == 0)
					return (it);
        }
        else 
        {
            if(((*it).isActive==true) &&(*it).val.funcVal->scope==scopeToLook  &&
			  (*it).val.funcVal->name.compare(nameToLook) == 0)
					return (it);
        }
}

void PrintSymbolTable(){
	list<Function_Argument*>::iterator iter;
	
    for(multimap<string,Symbol* >::iterator it =sTable.begin(); it !=sTable.end() ; ++it){
        if(it==sTable.begin())
            cout<<"\n\n";
					Symbol * its=(*it).second;
		if(its->scopeType==VARIABLE){
					cout<<  std::setw(10)   <<  "Variable: "
                        <<  std::setw(10)   <<  its->val.varVal->name
						<<  std::setw(7)    <<  "\tType: ";
                       if(its->symbolType==GLOBAL)
                       cout <<  std::setw(20)   <<"GLOBAL";
                       if(its->symbolType==LOCAL)
                       cout <<  std::setw(20)   <<"LOCAL";
                       if(its->symbolType==LOCAL_IN_FUNC)
                       cout <<  std::setw(20)   <<"LOCAL_IN_FUNCTION";
                       if(its->symbolType==FORMAL)
                       cout <<  std::setw(20)   <<"FORMAL";
					   cout << std::setw(7) << "\tLine: "
						   << std::setw(4) << its->val.varVal->getLine()
						   << std::setw(8) << "\tScope: "
						   << std::setw(5) << its->val.varVal->getScope()
						   << std::setw(8) << "\tOffset: "
						   << std::setw(5) << its->val.varVal->offset
                        <<  std::setw(1)    <<  "\n";
		}

		else if(its->scopeType==FUNCTION){
			
			cout<<setw(10)   <<  "Function: "
                            <<  setw(10)   << its->val.funcVal->getName() 
                            <<  setw(7)    <<  " Type: ";
            if(its->symbolType==USERFUNC){
                cout<< setw(9)    <<  "USER_FUNC"
					<< setw(11)   <<  " Arguments: ";
			}
            else if(its->symbolType==LIB_FUNC){
				cout<< setw(9)    <<  "LIB_FUNC"
					<< setw(11)   <<  " Arguments: ";
			}
			for ( iter=its->val.funcVal->getArguments().begin();
				iter!=its->val.funcVal->getArguments().end(); ++iter )
						cout<<   " "    <<  (*iter)->name;
                        cout<<  std::setw(8)    <<  ""
                            <<  std::setw(7)    <<  " Line: "
                            <<  std::setw(4)    <<  its->val.funcVal->getLine()
                            <<  std::setw(8)    <<  " Scope: "
                            <<  std::setw(5)    <<  its->val.funcVal->getScope()
							<< std::setw(8) << "\tTotalcalls_Iaddress: "
							<< std::setw(5) << its->val.funcVal->iadress
                            <<  std::setw(1)    <<  "\n";
		}
	}
}
void initSymbolTable(){
	// Insert the language-specific reserved Symbols
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("print",ss2,0,0)));
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("input",ss2,0,0)));
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("objectmemberkeys",ss2,0,0)));
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("objecttotalmembers",ss2,0,0)));
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("objectcopy",ss2,0,0)));
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("totalarguments",ss2,0,0)));
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("argument",ss2,0,0)));
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("strtonum",ss2,0,0)));
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("sqrt",ss2,0,0)));
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("cos",ss2,0,0)));
	lib_func.push_back(new Symbol(LIB_FUNC,new Function("sin",ss2,0,0)));
}
void Hide (int   sHide){
	int i=0;
	typedef	vector<Symbol*> mySymbol;mySymbol q;
	q=scpVec[sHide+1];
	
	if ((scpVec.size() < sHide+1)) 
		cerr   <<  "--> Scope : "<<  sHide<<  " doesn't exist in scope table"<<  " !!\n";

	else{
		for(i=0;i<q.size();i++){
            q[i]->isActive = false;
            if ( q[i]->val.varVal )
				cout   <<  q[i]->val.varVal->name << " egine hide \n";
        }
	}
}

void scpInsert(unsigned   scope, Symbol* ptr){
    if(scpVec.size()<scope+2)
     scpVec.resize(2+scope);
    scpVec.at(scope).push_back(ptr);
}
//int main() {
//	cout << "OK\n";
//	return 0;
//}
