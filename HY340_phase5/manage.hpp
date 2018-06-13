
#include "quads.hpp"
//#include <list>
/*Symbol* tmpSymbol;
	string functName="UNDEFINED_USER_fUNCTION";
 int scope;
	 int loops_number;
	 int loop=0;
	 bool funct=false;
	typedef list<Function_Argument*> arg ;arg ss;//lista gia ta arguments 
*/
	 
typedef struct lval{
      string id;
      idtype type;
      expr_*  member;
	}lval;

expr_* equal(iopcode op, expr_* ex1, expr_* ex2, int sc, int ln, unsigned int off);

Symbol *term_lvalue(string tempSymboll, int scp);
expr_ *  DOUBLE_PLUS_MINUS_lvalue(expr_ * lvalue,char op,int yylineno);

expr_ *  lvalue_DOUBLE_PLUS_MINUS(expr_ * lvalue, char op, int yylineno);

expr_ *lvalue_IDENT(string tempId, int yylineno, int scopes, int func, int offsets); 

expr_ *lvalue_LOCAL_IDENT(string tempId, int yylineno, int scope, int func, int offsets);

void insertFormalArgs(list<Function_Argument*> ss2,int y,int scope );
Symbol *insertNewLvalue(lval *lVal, int line, scopespace_t space, unsigned offse);
expr_* lvalue_DOUBLE_COLON_IDENT(string tempId, int yylineno, int scope, int func);

Symbol* FUNCTION_WITHOUT_IDENT(int yylineno, int scope, int func);

Symbol* FUNCTION_WITHOUT_IDENT(string tempId,list<Function_Argument*> ss2 ,int yylineno, int scope, int func);
Symbol * idlist_IDENT(string tempId, int scope, int yylineno, int func);
//=====================================================================================

Symbol *insertNewLvalue(lval *lVal, int line, scopespace_t space, unsigned offse);

Symbol *lvalue_IDENT(string tempId, int yylineno, int scopes, int func, int offsets,int a);
Symbol *lvalue_LOCAL_IDENT(string tempId, int yylineno, int scope, int func, int offsets,int a);
Symbol* lvalue_DOUBLE_COLON_IDENT(string tempId, int yylineno, int scope, int func,int a);

expr_ *newLvalue(lval *lVal, int line, scopespace_t space, unsigned offset);

expr_ *newArgument(string id, int line, unsigned offsetq);

expr_ * arithmetic(iopcode op, expr_* ex1, expr_* ex2, int sc, int ln, unsigned int off);

expr_*  COMPERATION(iopcode op, expr_* ex1, expr_* ex2, int sc, int ln, unsigned int off); 
expr_*  boolean(iopcode op, expr_* ex1, expr_* ex2, int ln);

expr_ *idlist_Ident(string id, int scope, int ln);
expr_ *idlist_Comma_Ident(string id, int scope, int ln);

//functionstack* put_funct_in_stack(functionstack*FunctionStack, string tmp, int scop, Symbol*newsymbol, int t);

expr_ *function_(int ln, Symbol *newsymbol);
expr_ *_function_(int yylineno, Symbol* newsymbol);

//functionstack* funcdef(functionstack* FunctionStack); 

expr_*	lvalue_callsufix(callsuffix_ *a, expr_ *lValue, expr_ *self);






