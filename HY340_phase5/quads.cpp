#include "quads.hpp"


quad*           quads = (quad*)0;
unsigned        total = 0;
unsigned int    currQuad = 0;
int SCope=1, line;

int tempcounter = 0, yylineno_ = 0;
int counterForTempName = 1;
unsigned programVarOffset = -1;
unsigned functionLocalOffset = 0;
unsigned formalArgOffset = 0;
unsigned scopeSpaceCounter = 1;

           

void comperror(string s)
{
	exit(EXIT_FAILURE);
}

int currscope() {
	return SCope;
}

void setScopeLine(int sc, int ln) {

	SCope = sc;
	line = ln;
}

scopespace_t currscopespace(void)
{
	if (scopeSpaceCounter == 1)    return programvar;
	else if (scopeSpaceCounter % 2 == 0)    return formalarg;
	else                                     return functionlocal;
}

unsigned currscopeoffset(void)
{
	switch (currscopespace())
	{
	case programvar:  return programVarOffset;
	case functionlocal:  return functionLocalOffset;
	case formalarg:  return formalArgOffset;
	default:  assert(0);
	}
}

unsigned enterscopespace(void)
{
	return ++scopeSpaceCounter;
}

void exitscopespace(void)
{
	//assert(scopeSpaceCounter>1); 
	--scopeSpaceCounter;
}



unsigned incurrscopeoffset(void)
{
	switch (currscopespace())
	{
	case programvar: return ++programVarOffset;    break;
	case functionlocal:return  ++functionLocalOffset; break;
	case formalarg: return  ++formalArgOffset;     break;
	default:  assert(0);
	}
}


string newtempname(void) {
	string tmp;
	char* tmp2 = (char*)malloc(sizeof(char) * 25);
	sprintf(tmp2, "_t%d", counterForTempName++);
	tmp = (*new string(tmp2));
	return tmp;
}

unsigned nextquadlabel(void) {
	//cout << "nextquadlabel delete currQuad " << currQuad << endl;
	return currQuad;
}


void expand(void)
{
	quad * p;
	assert(total == currQuad);
	p = (quad *)malloc(NEW_SIZE);
	if (quads)
	{
		memcpy(p, quads, CURR_SIZE);
		free(quads);
	}
	quads = p;
	total += EXPAND_SIZE;
}

expr_* newexpr(expr_t t) {
	expr_* e = new expr_();
	e->type = t;
	return e;
}

expr_* newexpr_conststring(string s) {
	expr_* e = newexpr(conststring_e);
	e->strConst = s;
	return e;
}
																		
expr_* newexpr_constnum(double i) {
	expr_* e = newexpr(constnum_e);
	e->numConst = i;
	return e;
}

expr_* newexpr_constbool(bool b) {
	expr_* e = newexpr(constbool_e);
	e->boolConst = b;
	return e;
}
int getLine(int  lin) {
	line = lin;
}
int getScope(int scop) {
	SCope = scop;
}
     extern unsigned VariableOffset(void){
	return programVarOffset;
}

quad *getQuad(){
	return quads;
	}

Symbol* newtemp() {
	Symbol*   tbl;
	string         name;
	//cout << name << " temp\n";
	name = newtempname();
	tbl = LookUp(name, SCope);
	if (!tbl) {
		if (currscopespace() == programvar) {
			if (SCope == 0) {
				tbl = new Symbol(GLOBAL, (new Variable(name, SCope, line, currscopeoffset() )), NULL);
				
			}
			else {
				tbl = new Symbol(LOCAL, (new Variable(name, SCope, line, currscopeoffset() - 1)), NULL);
			}

		}
		else if (currscopespace() == functionlocal) {
			tbl = new Symbol(LOCAL_IN_FUNC, (new Variable(name, SCope, line, currscopeoffset() - 1)), NULL);
			//incurrscopeoffset();
		}

		else if (currscopespace() == formalarg) {
			tbl = new Symbol(FORMAL, (new Variable(name, SCope, line, currscopeoffset() - 1)), NULL);
			//incurrscopeoffset();

		}
		Insert(tbl);
		scpInsert(SCope, tbl);
	}
	return tbl;
}


void checkuminus(expr_ *e)
{
	if (e->type == constbool_e ||
		e->type == conststring_e ||
		e->type == nil_e ||
		e->type == newtable_e ||
		e->type == programfunc_e ||
		e->type == libraryfunc_e ||
		e->type == boolexpr_e
		) {
		comperror("Illegal expression to unary - ");
	}
}


void emit(iopcode op, expr_ *arg1, expr_ *arg2, expr_ *result, unsigned label, unsigned line)
{
	quad *p;
	static unsigned count = 0;

	if (currQuad == total)
		expand();

	p = quads + currQuad++;
	p->op = op;
	p->arg1 = arg1;
	p->arg2 = arg2;
	p->result = result;
	p->label = label;
	p->line = line;
}

expr_ * emit_iftableitem(expr_ * e)
{
	//if (e == NULL)
		//return NULL;
	expr_ * result;
	if (e->type != tableitem_e)
		return  e;
	else
	{

		result = newexpr(var_e);
		incurrscopeoffset();
		result->sym = newtemp();

		emit(tablegetelem, e, e->index, result, nextquadlabel(), yylineno_);
	}

	return result;
}


expr_*   lvalue_expr(Symbol* sym) {
	expr_* e = (expr_*)malloc(sizeof(expr_));
	e->sym = sym;
	if (sym->symbolType == GLOBAL)
		e->type = var_e;
	else if (sym->symbolType == LOCAL)
		e->type = var_e;
	else if (sym->symbolType == FORMAL)
		e->type = var_e;
	else if (sym->symbolType == LOCAL_IN_FUNC)
		e->type = var_e;
	else if (sym->symbolType == USERFUNC)
		e->type = programfunc_e;
	else if (sym->symbolType == LIB_FUNC)
		e->type = libraryfunc_e;
	return e;
}



expr_* make_call(expr_* lvalue, list<expr_*> elist) {
	expr_*   func = emit_iftableitem(lvalue);

	expr_*   rslt;int i = 0;
	if (!elist.empty()) {
		list<expr_*>::reverse_iterator it;
		for (it = elist.rbegin(); it != elist.rend(); ++it) {
			cout << "bika sto parameter " << i++ << endl;
			//cout << (*it)->sym->val.varVal->offset<<endl;
			
			emit(param, (*it), (expr_*)0, (expr_*)0, nextquadlabel(), line);
		}
	}
	emit(call, func, (expr_*)0, (expr_*)0, nextquadlabel(), line);
	incurrscopeoffset();
	rslt = newexpr(var_e);
	rslt->sym = newtemp();
	emit(getretval, rslt, (expr_*)0, (expr_*)0, nextquadlabel(), line);
	return rslt;
}

expr_*  member_item(expr_* lvalue, string name) {
	lvalue = emit_iftableitem(lvalue);
	expr_* tableItem = newexpr(tableitem_e);
	tableItem->sym = lvalue->sym;
	tableItem->index = newexpr_conststring(name);
	return tableItem;
}

void patchlabel(unsigned quadNo, unsigned label) {
	//assert( quadNo < currQuad );
	quads[quadNo].label = label;
}

void patchlListlabels (list<int>vec, unsigned label){
  list<int>::iterator it;
  for ( it=vec.begin(); it!=vec.end(); ++it ){
      patchlabel(*it, label);
	}
}
void resetformalargsoffset(void) {
	formalArgOffset = 0;
}
void resetfunctionlocalsoffset(void) {
	functionLocalOffset = 0;
}

void resettemp(int tmpToResetTo) {
	counterForTempName = tmpToResetTo;
}
void restorecurrscopeoffset(unsigned oldoffset) {
	switch (currscopespace()) {
	case    programvar:   programVarOffset = oldoffset;  break;
	case    functionlocal:   functionLocalOffset = oldoffset;  break;
	case    formalarg:   formalArgOffset = oldoffset;  break;
	default:   assert(false);
	}
}


//==========================================================================
void    PrintExpr(expr_* expr) {
	if (expr->type == var_e) {
		if (expr->sym->scopeType == FUNCTION)
			cout << left << setw(10) << expr->sym->val.funcVal->name;
		else if (expr->sym->scopeType == VARIABLE)
			cout << left << setw(10) << expr->sym->val.varVal->name;
	}
	else if (expr->type == newtable_e) {
		if (expr->sym->scopeType == FUNCTION)
			cout << left << setw(10) << expr->sym->val.funcVal->name;
		else if (expr->sym->scopeType == VARIABLE)
			cout << left << setw(10) << expr->sym->val.varVal->name;
	}
	else if (expr->type == constnum_e)
		cout << left << setw(10) << expr->numConst;
	else if (expr->type == constbool_e) {
		if (expr->boolConst)
			cout << left << std::setw(10) << "True";
		else cout << left << setw(10) << "False";
	}
	else if (expr->type == conststring_e)
		cout << left << setw(10) << expr->strConst ;
	else if (expr->type == nil_e)
		cout << left << setw(10) << "Nill";
	else if (expr->type == tableitem_e)
		cout << left << setw(10) << expr->sym->val.varVal->name;
	else if (expr->type == programfunc_e)
		cout << left << setw(10) << expr->sym->val.funcVal->name;
	else if (expr->type == libraryfunc_e)
		cout << left << setw(10) << expr->strConst;
	else if (expr->type == arithexpr_e)
		cout << left << setw(10) << expr->sym->val.varVal->name;
	else if (expr->type == boolexpr_e)
		cout << left << setw(10) << expr->sym->val.varVal->name;
}

void   PrintQuads() {
	int     counter = 0;
	expr_*   nope;

	cout << "\n\n================== Printing Quads ==================\n\n";
	cout << setw(10) << "index" << setw(15) << "opcode" << setw(10) << "argument1" << setw(10)
		<< "argument2" << setw(10) << "result/label";
	cout << "\n_________________________________________________________\n\n";
	while (counter<currQuad) {
		if (quads[counter].op == assign) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "ASSIGN";
			PrintExpr(quads[counter].arg1);
			nope = newexpr_conststring(*new std::string(""));
			PrintExpr(nope);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}

		else if (quads[counter].op == add) {
			cout << left << std::setw(10) << counter;
			cout << left << std::setw(15) << "ADD";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == sub) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "SUB";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}

		else if (quads[counter].op == mul) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "MUL";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == division) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "DIV";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			PrintExpr(quads[counter].result);
			cout << "\n________________________________________________________\n\n";
		}
		else if (quads[counter].op == mod) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "MOD";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == uminus) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "UMINUS";
			PrintExpr(quads[counter].arg1);
			nope = newexpr_conststring(*new std::string(""));
			PrintExpr(nope);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == and_) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "AND";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == or_) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "OR";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == not_) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "NOT";
			PrintExpr(quads[counter].arg1);
			nope = newexpr_conststring(*new std::string(""));
			PrintExpr(nope);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == if_eq) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "IF_EQ";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			cout << left << setw(10) << quads[counter].label;
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == if_noteq) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "IF_NOT_EQ";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			cout << left << setw(10) << quads[counter].label;
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == if_lesseq) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "IF_LESS_EQ";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			cout << std::left << std::setw(10) << quads[counter].label;
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == if_greatereq) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "IF_GREATER_EQ";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			cout << left << setw(10) << quads[counter].label;
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == if_less) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "IF_LESS";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			cout << left << setw(10) << quads[counter].label;
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == if_greater) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "IF_GREATER";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			cout << left << setw(10) << quads[counter].label;
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == call) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "CALL";

			if (quads[counter].arg1->sym) {
				if (quads[counter ].arg1->sym->scopeType == FUNCTION)
				{
						cout << left
						<< setw(10)
						<< quads[counter ].arg1->sym->val.funcVal->name;
				}
				else if (quads[counter ].arg1->sym->scopeType == VARIABLE) {
					cout << left
						<< setw(10)
						<< quads[counter ].arg1->sym->val.varVal->name;
				}
				else if (quads[counter].arg1->type == libraryfunc_e) {
					cout << left
						<< setw(10)
						<< quads[counter].arg1->strConst;
				}

			}
			else if (quads[counter ].arg1->type == libraryfunc_e) {
				PrintExpr(quads[counter].arg1);
			}

			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == param) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "PARAM";
			PrintExpr(quads[counter].arg1);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == ret) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "RET";
			if (quads[counter].arg1)
				PrintExpr(quads[counter].arg1);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == getretval) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "GETRETVAL";
			PrintExpr(quads[counter].arg1);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == funcstart) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "FUNCSTART";
			if (quads[counter].arg1->sym->scopeType == FUNCTION) {
				cout << left << setw(10) << quads[counter].arg1->sym->val.funcVal->name;
			}
			else if (quads[counter].arg1->sym->scopeType == VARIABLE) {
				cout << left << setw(10) << quads[counter].arg1->sym->val.varVal->name;
			}
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == funcend) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "FUNCEND";
			if (quads[counter].arg1->sym->scopeType == FUNCTION) {
				cout << left << setw(10) << quads[counter].arg1->sym->val.funcVal->name;
			}
			else if (quads[counter].arg1->sym->scopeType == VARIABLE) {
				cout << left << setw(10) << quads[counter].arg1->sym->val.varVal->name;
			}
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == tablecreate) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "TABLECREATE";
			PrintExpr(quads[counter].arg1);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == tablegetelem) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "TABLEGETELEM";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == tablesetelem) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "TABLESETELEM";
			PrintExpr(quads[counter].arg1);
			PrintExpr(quads[counter].arg2);
			PrintExpr(quads[counter].result);
			cout << "\n_________________________________________________________\n\n";
		}
		else if (quads[counter].op == jump) {
			cout << left << setw(10) << counter;
			cout << left << setw(15) << "JUMP";
			nope = newexpr_conststring(*new std::string(""));
			PrintExpr(nope);
			PrintExpr(nope);
			cout << left << setw(10) << quads[counter].label;
			cout << "\n_________________________________________________________\n\n";
		}
		++counter;
	}
}
