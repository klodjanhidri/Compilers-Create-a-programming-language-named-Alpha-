#include "manage.hpp"

int funct1 = 0;
bool mesa_sto_fnc = false;
string functName1 = "$FUNCTION";
typedef list<Function_Argument*> arg11;
arg11 ss1;
Symbol * tmpSymbol;

//void setScopeLine(int sc, int ln) {

//	yylineno_ = ln;
//	scope = sc;
//}
Symbol *term_lvalue(string tempSymboll, int scp) {
	Symbol *a = LookUp(tempSymboll, scp);
	if (a == NULL)
		return NULL;
	if (a->scopeType == FUNCTION || a->symbolType == LIB_FUNC) {
		return a;
	}
	else 	return NULL;
}

expr_ *  DOUBLE_PLUS_MINUS_lvalue(expr_ * lvalue, char op, int yylineno) {
	expr_ * term, *val;
	switch (op) {
	case '+':
		if (lvalue->type == tableitem_e) {
			val = emit_iftableitem(lvalue);
			emit(add, val, newexpr_constnum(1), val, nextquadlabel(), yylineno);
			emit(tablesetelem, lvalue, lvalue->index, val, nextquadlabel(), yylineno);
			return val;
		}
		else {
			emit(add, lvalue, newexpr_constnum(1), lvalue, nextquadlabel(), yylineno);
			term = newexpr(arithexpr_e);
			//incurrscopeoffset();

			term->sym = newtemp();
			emit(assign, lvalue, NULL, term, nextquadlabel(), yylineno);
			return term;
		}
	case '-':
		if (lvalue->type == tableitem_e) {
			val = emit_iftableitem(lvalue);
			emit(sub, val, newexpr_constnum(1), val, nextquadlabel(), yylineno);
			emit(tablesetelem, lvalue, lvalue->index, val, nextquadlabel(), yylineno);
			return val;
		}
		else {
			emit(sub, lvalue, newexpr_constnum(1), lvalue, nextquadlabel(), yylineno);
			term = newexpr(arithexpr_e);
			//incurrscopeoffset();

			term->sym = newtemp();
			//incurrscopeoffset();
			emit(assign, lvalue, NULL, term, nextquadlabel(), yylineno);
			return term;
		}
	}return NULL;
}
expr_ *  lvalue_DOUBLE_PLUS_MINUS(expr_ * lvalue, char op, int yylineno) {
	expr_* term, *val;
	switch (op) {
	case '+':
		val = newexpr(var_e);
		//incurrscopeoffset();

		val->sym = newtemp();

		if (lvalue->type == tableitem_e) {
			expr_*   rslt = emit_iftableitem(lvalue);
			emit(assign, rslt, NULL, val, nextquadlabel(), yylineno);
			emit(add, rslt, newexpr_constnum(1), rslt, nextquadlabel(), yylineno);
			emit(tablesetelem, lvalue, lvalue->index, rslt, nextquadlabel(), yylineno);
			return val;
		}
		else {
			emit(assign, lvalue, NULL, val, nextquadlabel(), yylineno);
			emit(add, lvalue, newexpr_constnum(1), lvalue, nextquadlabel(), yylineno);
			return val;
		}
	case '-':
		term = newexpr(var_e);
		//incurrscopeoffset();

		term->sym = newtemp();

		if (lvalue->type == tableitem_e) {
			val = emit_iftableitem(lvalue);
			emit(assign, val, NULL, term, nextquadlabel(), yylineno);
			emit(sub, val, newexpr_constnum(1), val, nextquadlabel(), yylineno);
			emit(tablesetelem, lvalue, lvalue->index, val, nextquadlabel(), yylineno);
			return term;
		}
		else {
			emit(assign, lvalue, NULL, term, nextquadlabel(), yylineno);
			emit(sub, lvalue, newexpr_constnum(1), lvalue, nextquadlabel(), yylineno);

			return term;
		}
	}

	return NULL;


}

expr_ *lvalue_IDENT(string tempId, int yylineno, int scopes, int func, int offsets) {
	int scope = scopes;
	expr_* rslt;
	funct1 = func;
	while (scope >= 0)
	{
		if ((tmpSymbol = LookUp(tempId, scope)) != NULL) {
			if (isLibSymbol(tempId) == true) {
				cout << "this Lib_Function " << tempId << " is previous declared \n";
				return 0;
				rslt = newexpr(var_e);
				rslt->sym = tmpSymbol;
				return rslt;
			}

			switch (tmpSymbol->symbolType) {
			case GLOBAL: {cout << "this Global var " << tempId << " is previous declared \n";
				rslt = newexpr(var_e);
				rslt->sym = tmpSymbol;
				return rslt;
			}
			case FORMAL: {if (mesa_sto_fnc) {
				mesa_sto_fnc = false;
				cerr << "ERROR access a LOCAL_VAR, OUTSIDE of the current function " << tempId << endl;
			}
						 else {
							 cout << "this FORMAL var " << tempId << " is previous declared at scope" << scope << endl;
							 rslt = newexpr(var_e);
							 rslt->sym = tmpSymbol;
							 return rslt;
						 }}

			case LOCAL_IN_FUNC: {if (mesa_sto_fnc) {
				mesa_sto_fnc = false;
				cerr << "ERROR access a LOCAL_VAR_IN_FUNC, OUTSIDE of the current function " << tempId << endl;
			}
								else {
									cout << "this local_in_func var " << tempId << " is previous declared at scope " << scope;
									rslt = newexpr(var_e);
									rslt->sym = tmpSymbol;
									return rslt;
								}}
			case USERFUNC: { cout << "this user function " << tempId << " is previous declared at scope " << scope << endl;
				rslt = newexpr(var_e);
				rslt->sym = tmpSymbol;
				return rslt;
			}
			case LOCAL: {if (mesa_sto_fnc) {
				mesa_sto_fnc = false;
				cerr << "ERROR access a LOCAL_VAR, OUTSIDE of the current function " << tempId << endl;
			}
						else {
							cout << "this local_var " << tempId << " is previous declared at scope " << scope << endl;
							rslt = newexpr(var_e);
							rslt->sym = tmpSymbol;
							return rslt;
						}}

			}
		}

		if (!tmpSymbol) {
			scope--;
			if (funct1)mesa_sto_fnc = true;
		}
		else { mesa_sto_fnc = false;  break; }
	}
	if (scope == -1)scope = 0;
	if (!tmpSymbol) {
		if (isLibSymbol(tempId)) {
			cout << "The symbol is LIB_FUNC " << tempId << "\n";
			rslt = newexpr(libraryfunc_e);
			rslt->strConst = tempId;
			return rslt;
		}else {
		//	incurrscopeoffset();
			if (scopes == 0)
				Insert(tmpSymbol = new Symbol(GLOBAL, new Variable(tempId, scopes, yylineno, offsets), NULL));
			else {
				if (funct1)
					Insert(tmpSymbol = new Symbol(LOCAL_IN_FUNC, new Variable(tempId, scopes, yylineno, offsets), NULL));
				else
					Insert(tmpSymbol = new Symbol(LOCAL, new Variable(tempId, scopes, yylineno, offsets), NULL));

			}
			incurrscopeoffset();
			scpInsert(scopes, tmpSymbol);
			rslt = newexpr(var_e);
			rslt->sym = tmpSymbol;
			return rslt;
		}
	}
	return rslt;
}

expr_ *lvalue_LOCAL_IDENT(string tempId, int yylineno, int scope, int func, int offsets) {
	funct1 = func;
	expr_* rslt;

	if (((tmpSymbol = LookUp(tempId, scope))) != NULL) {
		if (tmpSymbol->symbolType == USERFUNC || tmpSymbol->symbolType == LIB_FUNC) {
			cerr << "Error " << tempId << " is re-declareting function as LOCAL_VAR\n";
		}
		else {
			cout << "Reference to LOCAL_VAR :" << tempId << " .\n";
			rslt = newexpr(var_e);
			rslt->sym = tmpSymbol;
			return rslt;
		}
	}
	else
	{
		if (isLibSymbol(tempId) == true) {
			cerr << "Error " << tempId << " is lib function\n";
		}
		

		if (scope == 0) {
			Insert((tmpSymbol = new Symbol(GLOBAL, new Variable(tempId, scope, yylineno, offsets), NULL)));
		}
		else
		{
			if (funct1)
				Insert(tmpSymbol = new Symbol(LOCAL_IN_FUNC, new Variable(tempId, scope, yylineno, offsets), NULL));
			else

				Insert(tmpSymbol = new Symbol(LOCAL, new Variable(tempId, scope, yylineno, offsets), NULL));
		}
		incurrscopeoffset();
		scpInsert(scope, tmpSymbol);
		rslt = newexpr(var_e);
		rslt->sym = tmpSymbol;
		return rslt;
	}
	return rslt;
}



expr_* lvalue_DOUBLE_COLON_IDENT(string tempId, int yylineno, int scope, int func) {
	funct1 = func;
	expr_* rslt;

	tmpSymbol = LookUp(tempId, 0);
	if (tmpSymbol) {

		if (tmpSymbol->symbolType == USERFUNC || tmpSymbol->symbolType == LIB_FUNC) {
			cout << " GLOBAL_FUNCT : <" << tempId << ">  !!!\n";

		}
		else {
			cout << "GLOBAL_VAR   : <" << tempId << "> \n";

		}
		rslt = newexpr(var_e);
		rslt->sym = tmpSymbol;
		return rslt;
	}
	return NULL;

}
//=======================================================================================


Symbol *lvalue_IDENT(string tempId, int yylineno, int scopes, int func, int offsets,int a ) {
	int scope = scopes;
	expr_* rslt;
	funct1 = func;
	while (scope >= 0)
	{
		if ((tmpSymbol = LookUp(tempId, scope)) != NULL) {
			if (isLibSymbol(tempId) == true) {
				cout << "this Lib_Function " << tempId << " is previous declared \n";
				return 0;
				
				
			}

			switch (tmpSymbol->symbolType) {
			case GLOBAL: {cout << "this Global var " << tempId << " is previous declared \n";
				rslt = newexpr(var_e);
				rslt->sym = tmpSymbol;
				
			}
			case FORMAL: {if (mesa_sto_fnc) {
				mesa_sto_fnc = false;
				cerr << "ERROR access a LOCAL_VAR, OUTSIDE of the current function " << tempId << endl;
			}
						 else {
							 cout << "this FORMAL var " << tempId << " is previous declared at scope" << scope << endl;
							 rslt = newexpr(var_e);
							 rslt->sym = tmpSymbol;
							 
						 }}

			case LOCAL_IN_FUNC: {if (mesa_sto_fnc) {
				mesa_sto_fnc = false;
				cerr << "ERROR access a LOCAL_VAR_IN_FUNC, OUTSIDE of the current function " << tempId << endl;
			}
								else {
									cout << "this local_in_func var " << tempId << " is previous declared at scope " << scope;
									rslt = newexpr(var_e);
									rslt->sym = tmpSymbol;
									
								}}
			case USERFUNC: { cout << "this user function " << tempId << " is previous declared at scope " << scope << endl;
				rslt = newexpr(var_e);
				rslt->sym = tmpSymbol;
				
			}
			case LOCAL: {if (mesa_sto_fnc) {
				mesa_sto_fnc = false;
				cerr << "ERROR access a LOCAL_VAR, OUTSIDE of the current function " << tempId << endl;
			}
						else {
							cout << "this local_var " << tempId << " is previous declared at scope " << scope << endl;
							rslt = newexpr(var_e);
							rslt->sym = tmpSymbol;
							return tmpSymbol;
						}}

			}
		}

		if (!tmpSymbol) {
			scope--;
			if (funct1)mesa_sto_fnc = true;
		}
		else { mesa_sto_fnc = false;  break; }
	}
	if (scope == -1)scope = 0;
	if (!tmpSymbol) {
		if (isLibSymbol(tempId)) {
			cout << "The symbol is LIB_FUNC " << tempId << "\n";
			
			return NULL;
		}
		else {
			
			if (scopes == 0)
				Insert(tmpSymbol = new Symbol(GLOBAL, new Variable(tempId, scopes, yylineno, offsets), NULL));
			else {
				if (funct1)
					Insert(tmpSymbol = new Symbol(LOCAL_IN_FUNC, new Variable(tempId, scopes, yylineno, offsets), NULL));
				else
					Insert(tmpSymbol = new Symbol(LOCAL, new Variable(tempId, scopes, yylineno, offsets), NULL));

			}
			//incurrscopeoffset();
			scpInsert(scopes, tmpSymbol);
			rslt = newexpr(var_e);
			rslt->sym = tmpSymbol;
			return tmpSymbol;
		}
	}
	return tmpSymbol;
}

Symbol *lvalue_LOCAL_IDENT(string tempId, int yylineno, int scope, int func, int offsets,int a) {
	funct1 = func;
	expr_* rslt;

	if (((tmpSymbol = LookUp(tempId, scope))) != NULL) {
		if (tmpSymbol->symbolType == USERFUNC || tmpSymbol->symbolType == LIB_FUNC) {
			cerr << "Error " << tempId << " is re-declareting function as LOCAL_VAR\n";
		}
		else {
			cout << "Reference to LOCAL_VAR :" << tempId << " .\n";
			rslt = newexpr(var_e);
			rslt->sym = tmpSymbol;
			return tmpSymbol;
		}
	}
	else
	{
		if (isLibSymbol(tempId) == true) {
			cerr << "Error " << tempId << " is lib function\n";
		}
		//incurrscopeoffset();

		if (scope == 0) {
			Insert((tmpSymbol = new Symbol(GLOBAL, new Variable(tempId, scope, yylineno, offsets), NULL)));
		}
		else
		{
			if (funct1)
				Insert(tmpSymbol = new Symbol(LOCAL_IN_FUNC, new Variable(tempId, scope, yylineno, offsets), NULL));
			else

				Insert(tmpSymbol = new Symbol(LOCAL, new Variable(tempId, scope, yylineno, offsets), NULL));
		}

		scpInsert(scope, tmpSymbol);
		rslt = newexpr(var_e);
		rslt->sym = tmpSymbol;
		return tmpSymbol;
	}
	return tmpSymbol;
}



Symbol* lvalue_DOUBLE_COLON_IDENT(string tempId, int yylineno, int scope, int func,int a) {
	funct1 = func;
	expr_* rslt;

	tmpSymbol = LookUp(tempId, 0);
	if (tmpSymbol) {

		if (tmpSymbol->symbolType == USERFUNC || tmpSymbol->symbolType == LIB_FUNC) {
			cout << " GLOBAL_FUNCT : <" << tempId << ">  !!!\n";

		}
		else {
			cout << "GLOBAL_VAR   : <" << tempId << "> \n";

		}
		rslt = newexpr(var_e);
		rslt->sym = tmpSymbol;
		return tmpSymbol;
	}
	return NULL;

}

//=======================================================================================
expr_ *newLvalue(lval *lVal, int line, scopespace_t space, unsigned offset) {
	Symbol *newsymbol = NULL;
	expr_ *lValue;
	newsymbol = insertNewLvalue(lVal, line, space, offset);
	if (newsymbol == NULL) {
		if (isLibSymbol(lVal->id)) {

			lValue = newexpr(libraryfunc_e);
			lValue->strConst = lVal->id;
			return lValue;
		}


	}
	//cout<< newsymbol->val.funcVal->name << "  dsadsad\n\n";
	lValue = NULL;
	if (lVal->type == MEMBER) {
		lValue = lVal->member;
		//if (!lValue) {

		//	return NULL;
		//}
	}
	else {
		if (!newsymbol) {
			return NULL;
		}
		lValue = (expr_*)malloc(sizeof(expr_));

		if (newsymbol->scopeType == VARIABLE) {
			lValue->type = var_e;
		}
		else {
			if (newsymbol->scopeType == FUNCTION) {
				lValue->type = programfunc_e;
			}
			else {
				lValue->type = libraryfunc_e;
			}

		}
		lValue->sym = newsymbol;
	}
	return lValue;
}
Symbol *insertNewLvalue(lval *lVal, int line, scopespace_t space, unsigned offse) {
	string identifier = lVal->id;
	

	idtype type = lVal->type;
	

	Symbol *newSymbolEntry = NULL;
	if (type == GLOBALID) {

		newSymbolEntry = lvalue_DOUBLE_COLON_IDENT(identifier, line, space,0,0);
	}
	else if (type == LOCALID) {
		newSymbolEntry = lvalue_LOCAL_IDENT(identifier, line, space, offse,0,0);
	}
	else if (type == SIMPLEID) {
		newSymbolEntry = lvalue_IDENT(identifier, line, space, offse,0,0);

	}
	else {
		newSymbolEntry = NULL;
	}
	return newSymbolEntry;
}

expr_ *newArgument(string id, int line, unsigned offsetq) {
	Symbol *newsymbol = NULL;
	expr_* formal;
	newsymbol = idlist_IDENT(id, line, offsetq,0);
	if (!newsymbol) { return NULL; }
	formal = (expr_ *)malloc(sizeof(expr_));
	formal->type = var_e;
	formal->sym = newsymbol;
	return formal;
}
Symbol* FUNCTION_WITHOUT_IDENT(int yylineno, int scope, int func) {
	expr_* rslt;

	funct1 = func;
	Insert((tmpSymbol = new Symbol(USERFUNC, NULL, new Function(functName1, ss1, scope, yylineno))));

	scpInsert(scope, tmpSymbol);
	list<Function_Argument*>::const_iterator it;
	for (it = ss1.begin(); it != ss1.end(); ++it) {
		//incurrscopeoffset();
		Insert((tmpSymbol = new Symbol(FORMAL, new Variable((*it)->name, scope + 1, yylineno, currscopeoffset()), NULL)));
		//cout << "FUNCTION_WITHO_IDNE" << scope << endl;

		scpInsert(scope + 1, tmpSymbol);
	}
	return tmpSymbol;

}


Symbol* FUNCTION_WITHOUT_IDENT(string tempId, list<Function_Argument*> ss2, int yylineno, int scope, int func) {
	funct1 = func;
	expr_* rslt;
	enterscopespace();
	resetfunctionlocalsoffset();
	tmpSymbol = LookUp(tempId, scope);
	if ((tmpSymbol != NULL) || (isLibSymbol(tempId) == true)) {
		cerr << "Error  Previous declaration/reference to " << tempId << "\n";
	}
	Insert((tmpSymbol = new Symbol(USERFUNC, NULL, (new Function(tempId, ss2, scope, yylineno)))));
	//cout << "FUNCTION_W_IDNE1" << scope << endl;

	scpInsert(scope, tmpSymbol);
	tmpSymbol->val.funcVal->iadress = nextquadlabel();
	tmpSymbol->symbolType = USERFUNC;
	emit(funcstart, lvalue_expr(tmpSymbol), NULL, NULL, nextquadlabel(), yylineno);
	list<Function_Argument*>::const_iterator it;

	
	//ss1.clear();
	//resetfunctionlocalsoffset();
	return tmpSymbol;
}

void insertFormalArgs(list<Function_Argument*> ss2,int y,int  scope) {
	list<Function_Argument*>::const_iterator it;
	
	Symbol *tmpSymbol;
	for (it = ss2.begin(); it != ss2.end(); ++it) {
		incurrscopeoffset();
		
		Insert((tmpSymbol = new Symbol(FORMAL, new Variable((*it)->name, scope + 1,y, currscopeoffset()), NULL)));
		scpInsert(scope + 1, tmpSymbol);
	}ss1.clear();resetformalargsoffset();

}
Symbol * idlist_IDENT(string tempId, int scope, int yylineno, int func) {
	cout << tempId << "\n";
	expr_* rslt;
	setScopeLine(scope, yylineno);
	//enterscopespace();          //now entering function locals space
	//resetformalargsoffset();    //start locals from zero
	funct1 = func;
	tmpSymbol = LookUp(tempId, scope);
	if ((isLibSymbol(tempId)))
	{
		cerr << "Error  Previous declaration/reference or (lib function) to  " << tempId << "\n";
	}

	else
	{
		ss1.push_back(new Function_Argument(tempId, 0));
	}

	return tmpSymbol;
}



expr_ * arithmetic(iopcode op, expr_* ex1, expr_* ex2, int sc, int ln, unsigned int off)
{
	expr_* rslt = newexpr(arithexpr_e);

	if (ex1->type == constnum_e && ex2->type == constnum_e) {
		rslt->type = constnum_e;
		if (op == add)
			rslt->numConst = ex1->numConst + ex2->numConst;
		else if (op == sub)
			rslt->numConst = ex1->numConst - ex2->numConst;
		else if (op == mul)
			rslt->numConst = ex1->numConst * ex2->numConst;
		else if (op == division)
			rslt->numConst = ex1->numConst / ex2->numConst;
		else if (op == mod)
			rslt->numConst = (int)ex1->numConst % (int)ex2->numConst;
	}
	else {
		incurrscopeoffset();
		rslt->sym = newtemp();
		emit(op, ex1, ex2, rslt, nextquadlabel(), ln);
	}
	return rslt;
}
expr_*  COMPERATION(iopcode op, expr_* ex1, expr_* ex2, int sc, int ln, unsigned int off) {
	expr_* rslt = newexpr(boolexpr_e);
	if (ex1->type == constnum_e && ex2->type == constnum_e) {
		rslt->type = constbool_e;
		if (op == if_greater)
			rslt->boolConst = (ex1->numConst > ex2->numConst);
		else if (op == if_greatereq)
			rslt->boolConst = (ex1->numConst >= ex2->numConst);

		else if (op == if_less)
			rslt->boolConst = (ex1->numConst < ex2->numConst);

		else if (op == if_lesseq)
			rslt->boolConst = (ex1->numConst <= ex2->numConst);

	}
	else {
		
		incurrscopeoffset();
		rslt->sym = newtemp();
		emit(op, ex1, ex2, NULL, nextquadlabel() + 3, ln);
		emit(assign, newexpr_constbool(false), NULL, rslt, nextquadlabel(), ln);
		emit(jump, NULL, NULL, NULL, nextquadlabel() + 2, ln);
		emit(assign, newexpr_constbool(true), NULL, rslt, nextquadlabel(), ln);
	}
	return rslt;
}

expr_*  boolean(iopcode op, expr_* ex1, expr_* ex2, int ln) {
	expr_* rslt;
	bool ev1, ev2;

	if ((ex1->type == var_e || ex1->type == assignexpr_e || ex1->type == boolexpr_e || ex1->type == arithexpr_e)
		&& (ex2->type == var_e || ex2->type == assignexpr_e || ex2->type == boolexpr_e || ex2->type == arithexpr_e))
	{
		incurrscopeoffset();
		rslt = newexpr(boolexpr_e);
		rslt->sym = newtemp();
		emit(op, ex1, ex2, rslt, nextquadlabel(), ln);
	}
	else {
		if (ex1->type == constnum_e) {
			if (ex1->numConst == 0)
				ev1 = false;
			else
				ev1 = true;
		}
		else if (ex1->type == constbool_e)
			ev1 = ex1->boolConst;

		else if (ex1->type == conststring_e) {
			if (ex1->strConst.compare("") == 0)
				ev1 = false;
			else
				ev1 = true;
		}
		else if (ex1->type == nil_e)
			ev1 = false;
		else if (ex1->type == newtable_e)
			ev1 = true;
		if (ex2->type == constnum_e) {
			if (ex2->numConst == 0)
				ev2 = false;
			else
				ev2 = true;
		}
		else if (ex2->type == constbool_e)
			ev2 = ex2->boolConst;

		else if (ex2->type == conststring_e) {
			if (ex2->strConst.compare("") == 0)
				ev2 = false;
			else
				ev2 = true;
		}
		else if (ex2->type == nil_e)
			ev2 = false;

		else if (ex2->type == newtable_e)
			ev2 = true;
		if (op == and_) {
			if (ev1 == true && ev2 == true) {
				rslt = newexpr_constbool(true);
			}
			else {
				rslt = newexpr_constbool(false);
			}
		}
		else if (op == or_) {
			if (ev1 == true || ev2 == true) {
				rslt = newexpr_constbool(true);
			}
			else {
				rslt = newexpr_constbool(false);
			}
		}
	}

	return rslt;
}

expr_ *idlist_Ident(string id,int scope,int ln) {
	setScopeLine(scope, ln);
	
	//enterscopespace();
	//resetformalargsoffset();
	expr_ *lValue = newArgument(id, ln, currscopeoffset());
	
	return lValue;
}

expr_ *idlist_Comma_Ident(string id, int scope, int ln) {
	setScopeLine(scope, ln);
	//enterscopespace();
	//resetformalargsoffset();
	//incurrscopeoffset();
	expr_ *lValue = newArgument(id, ln, currscopeoffset());
	
	return lValue;
}

expr_ *function_(int ln, Symbol *newsymbol) {
	expr_  *lValue = NULL;
	lValue = newexpr(programfunc_e);
	lValue->sym = newsymbol;
	emit(funcstart, lValue, NULL, NULL, nextquadlabel(), ln);
	return lValue;

}
expr_ *_function_(int yylineno, Symbol* newsymbol) {

	expr_ *lValue = newexpr(programfunc_e);
	lValue->sym = newsymbol;
	emit(funcstart, lValue, NULL, NULL, nextquadlabel(), yylineno);
	return lValue;

}
/*functionstack* funcdef(functionstack* FunctionStack) {
	functionstack* a;
	if (fStackTop() == NULL) {
		if (fStackTop()->funcdef == true) {
			tempcounter = topTempCounter();
			FunctionStack = fStackPop();
			FunctionStack->funcdef = false;
		}

	}a = FunctionStack;
	return a;
}
*/
expr_*	lvalue_callsufix(callsuffix_ *a, expr_ *lValue, expr_ *self) {
	expr_ *index, *lVal = emit_iftableitem(lValue);
	expr_		*item = newexpr(tableitem_e);
	item->sym = lVal->sym;
	index = newexpr_conststring(a->name);
	item->index = index;
	self = item;
	item = emit_iftableitem(item);
	if ((*(a->elist)).empty()) {
		a->elist = new list<expr_*>;
		(*(a->elist)).push_back(self);
	}
	else {
		(*(a->elist)).push_back(self);
	}
	item = make_call(item, (*(a->elist)));
	return item;
}

/*
expr_* endbackpatch(expr * exprVal, int line) {
	expr *tmpexpr;
	if ((exprVal->truelist != NULL) || (exprVal->falselist != NULL)) {
		printf("End Truelist:\n");
		List_Print(exprVal->truelist);
		printf("End FalseList:\n");
		List_Print(exprVal->falselist);
		if (exprVal->truelist != NULL) { backpatch(exprVal->truelist, nextquadlabel()); }
		if (exprVal->falselist != NULL) { backpatch(exprVal->falselist, nextquadlabel() + 2); }
		tmpexpr = newexpr(boolexpr_e);
		tmpexpr->sym = (newtemp());
		tmpexpr->truelist = NULL;
		tmpexpr->falselist = NULL;
		emit(assign, newexpr_constbool(TRUE), NULL, tmpexpr, nextquadlabel(), line);
		emit(jump, NULL, NULL, NULL, nextquadlabel() + 2, line);
		emit(assign, newexpr_constbool(FALSE), NULL, tmpexpr, nextquadlabel(), line);
		return tmpexpr;
	}
	printf("End NO LISTS!!!!!!!!!\n");
	return exprVal;
}*/