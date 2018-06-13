#ifndef QUADS_HPP_INCLUDED
#define QUADS_HPP_INCLUDED
/*	Name:	Klodjan Hidri
	AM:		2726
	login:	klodjan.hidri@gmail.com
*/

#include    <stdlib.h>
#include    <stdio.h>
#include    <assert.h>
#include    <string>
#include    <list>
#include    <stack>
#include    <string.h>
#define EXPAND_SIZE         1024
#include    "def.hpp"

typedef enum iopcode {
	assign = 1, add, sub, mul, division, mod,
	uminus, and_, or_, not_,
	if_eq, if_noteq, if_lesseq, if_greatereq, if_less, if_greater,
	call, param, ret, jump, getretval,
	funcstart, funcend,
	tablecreate, tablegetelem, tablesetelem
}iopcode;

enum    expr_t {
	var_e,
	tableitem_e,

	programfunc_e,
	libraryfunc_e,

	arithexpr_e,
	boolexpr_e,
	assignexpr_e,
	newtable_e,

	constnum_e,
	constbool_e,
	conststring_e,

	nil_e
};

struct  expr_ {
	expr_t              type;
	Symbol*   			sym;
	expr_*              index;
	double              numConst;
	string         		strConst;
	bool                boolConst;

};

typedef struct quad {
	iopcode    op;
	expr_    *  result;
	expr_    *  arg1;
	expr_    *  arg2;
	unsigned   label;
	unsigned   line;
	 unsigned    taddress; 

}quad;

#define CURR_SIZE ( total * sizeof(quad) )
#define NEW_SIZE  ( EXPAND_SIZE * sizeof(quad) + CURR_SIZE )


enum    scopespace_t {
	programvar,
	functionlocal,
	formalarg
};

enum    symbol_t { var_s, programfunc_s, libraryfunc_s };

struct  symbol {
	symbol_t        type;
	std::string     name;   //Dynamic string
	scopespace_t    space;  //Originating scope space
	unsigned        offset; //Offset in scope space
	unsigned        scope;  //Scope value
	unsigned        line;   //Source line of declaration

};

typedef struct statement
{
	list<int>  breaklist;
	list<int>  continuelist;
}statement_t;

typedef struct callsuffix_ {
	list<expr_*> *elist;
	bool method;
	string name;
}callsuffix_;

typedef struct forprefix_
{
	int test;
	int enter;
}forprefix_;


typedef enum idtype {
	SIMPLEID,
	GLOBALID,
	LOCALID,
	MEMBER
}idtype;


unsigned VariableOffset(void);
quad *getQuad();
void comperror(string s);
scopespace_t currscopespace(void);
unsigned currscopeoffset(void);
unsigned enterscopespace(void);
void exitscopespace(void);
unsigned incurrscopeoffset(void);
string newtempname(void);
unsigned nextquadlabel(void);
void expand(void);
expr_* newexpr(expr_t t);
expr_* newexpr_conststring(string s);
expr_* newexpr_constnum(double i);
expr_* newexpr_constbool(bool b);
int getLine(int  lin);
int getScope(int scop);
Symbol* newtemp();
void checkuminus(expr_ *e);

void emit(iopcode op, expr_ *arg1, expr_ *arg2, expr_ *result, unsigned label, unsigned line);

expr_ *emit_iftableitem(expr_ * e);
expr_*   lvalue_expr(Symbol* sym);
expr_* make_call(expr_* lvalue, list<expr_*> elist);
expr_*  member_item(expr_* lvalue, string name); 
void patchlabel(unsigned quadNo, unsigned label);
void patchlListlabels(list<int>vec, unsigned label);
void resetformalargsoffset(void); 
void resetfunctionlocalsoffset(void);
void resettemp(int tmpToResetTo);
void restorecurrscopeoffset(unsigned oldoffset);
void PrintExpr(expr_* expr);
void PrintQuads();
void setScopeLine(int sc, int ln);
int currscope();

#endif
