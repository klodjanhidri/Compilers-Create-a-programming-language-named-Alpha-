	/* 
       Fall 2016
       Compilers
        Phase III
    Author  :Hidri Klodjan
    Login  	:hidri
   parser.y:		
        Edit Date:08/03/2016
	*/
 
%{

	
	
	#include "manage.hpp"
	#include "generate.hpp"

	#include <stack>
	int scope=0;
    int loop=0;
    int funct=0;
    string functName="$";
	int yyerror (string yaccProvidedMessage);
	int yylex (void);
    typedef list<Function_Argument*> arg13 ;
          arg13 ss;
	statement_t*    statement=new statement_t(); 
	extern FILE* yyin;

	extern int yylineno;
	extern char* yytext;
	//lval *lVal =new lval();
	Symbol *newsymbol;

	 int F        = 0;
	int cc=1;                                               
    int Scope=0;
    extern int tmpcounter;
    stack<int> scopeoffsetstack;           
    std::list<expr_> argu;
    statement_t * currentLoop=NULL;
    expr_* lValue;
%}


%code requires { #include <string> }
%code requires { #include <list> }
%code requires { #include "def.hpp" }
%union{
	unsigned  intValue;
	char *stringValue;
	float floatValue;
	double  realValue;
	struct expr_ *exprVal;
	struct callsuffix_* callVal_;
	struct statement * stmtVal_;
	struct forprefix_ * forVal;
	std::list<expr_*>* argu;
	struct lval *lVal ;
	Symbol *aaa;
	 std::string*      stringName;
	}

%start 	program
%token <stringValue>	IDENT
%token <intValue> 	INTEGER 
%token <realValue>	DOUBLE
%token <realValue>  FLOAT
%token <stringValue>	STRING
%token <stringValue>ERROR


//funcprefix funcargs funcbody
		/*keywords*/
%token	IF ELSE WHILE FOR FUNCTION_ RETURN BREAK CONTINUE AND NOT OR LOCAL_ TRUE FALSE NIL
		/*operators*/
%token	ASSIGN PLUS MINUS MULTIPLY DIVISION PERSENT EQUAL EQUAL_NOT DOUBLE_PLUS DOUBLE_MINUS
%token 	GREATER GREATER_EQUAL LESS LESS_EQUAL
		/*punctuation marks*/
%token	OPEN_AGKILI CLOSE_AGKILI OPEN_PARENTHESI CLOSE_PARENTHESI
%token 	SEMICOLON COMMA COLON SCOPE DOT TWODOTS OPEN_BRACKET CLOSE_BRACKET
		/*illegal strings*/ 
%token	ILLEGAL_STRING_ESQ  ILLEGAL_STR

	
		/*  operator`s priority */
%left 		AND	OR ASSIGN
%nonassoc 	GREATER GREATER_EQUAL LESS LESS_EQUAL EQUAL EQUAL_NOT
%left 		PLUS MINUS
%left 		MULTIPLY DIVISION PERSENT
%right 		PRIORITY_MINUS DOUBLE_PLUS DOUBLE_MINUS NOT
%left 		OPEN_AGKILI CLOSE_AGKILI 
%left 		OPEN_PARENTHESI CLOSE_PARENTHESI

%type   <lVal>	lvalue	
%type	<exprVal>  term 	expr const primary objectdef  indexedelem member assignexpr 		call idlist funcargs //funcname elist indexed
%type	<callVal_>	callsuffix normcall methodcall
%type   <stmtVal_>	stmt stmts ifstmt loopstmt forstmt block
%type   <forVal>	forprefix
%type   <intValue> ifprefix elseprefix N M whilestart whilecond funcbody
%type 	<argu> elist indexed
%type<aaa> funcprefixAndArgs funcdef
%type<stringName>funcname

%%
program : stmts{};
stmts: stmts stmt  {}
	| {}
    ;
/*----------------------------------------------------------------------------------------*/
stmt	
    : expr SEMICOLON 	{}
		
    | ifstmt 		{}
		
    | whilestmt			{}
		
    | forstmt		 {}
		
	| returnstmt 	{
		if(funct==0)
	  yyerror("returnstmt is not inside to a function!!!!!");
	        }
        
	| BREAK SEMICOLON 		{
	cout<<loop<<" loop\n";
	 if(loop<=0)
	    yyerror("break is not inside a loop!!!!!!!");
		else{
                               
                              
								statement->breaklist.push_back(nextquadlabel());
                                emit(jump, NULL,NULL,NULL, -1, yylineno);
                                $$ = statement;
                            }
	    }
        
    | CONTINUE SEMICOLON  		{ 
	cout<<loop<<" loop\n";
	 if(loop<=0)
	    yyerror("continue is not inside a loop!!!!!!!");
	 else{
                         statement->continuelist.push_back(nextquadlabel());
                                emit(jump,NULL,NULL,NULL, -1, yylineno);
                                $$ = statement;
                            }
	
	}
        
    | block  		{$$ =  $1;}
		
    | funcdef 		{$$=NULL;}
		
    | SEMICOLON  {$$=NULL;}
    ;

/*----------------------------------------------------------------------------------------*/

expr
    : assignexpr		{ $$ = $1;}
    
    | expr PLUS expr		{$$ = arithmetic(add, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr MINUS expr		{$$ = arithmetic(sub, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr MULTIPLY expr		{$$ = arithmetic(mul, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr DIVISION expr		{$$ = arithmetic(division, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr PERSENT expr		{$$ = arithmetic(mod, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr GREATER expr		{$$ =  COMPERATION(if_greater, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr GREATER_EQUAL expr			{$$ =  COMPERATION(if_greatereq, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr LESS expr		{$$ =  COMPERATION(if_less, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr LESS_EQUAL expr		{$$ =  COMPERATION(if_lesseq, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr EQUAL expr	{$$ = equal(if_eq, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr EQUAL_NOT expr		{$$ = equal(if_noteq, $1, $3, scope, yylineno, currscopeoffset());}
    
    | expr AND expr			{$$ = boolean(and_, $1, $3,yylineno);}
    
    | expr OR expr		{$$ = boolean(or_, $1, $3,yylineno);}
    
    | term		{$$ = $1;}
    ;
/*----------------------------------------------------------------------------------------*/

term
    : OPEN_PARENTHESI expr CLOSE_PARENTHESI		{$$=$2;}
    
    | MINUS expr  %prec PRIORITY_MINUS   {expr_* rslt  = newexpr(arithexpr_e);
		setScopeLine(scope,yylineno);

	    if($2->sym&&$2->sym->symbolType == LOCAL&&
            ($2->sym->val.varVal->name.compare(0, 0, "_")))
            rslt->sym = $2->sym;
        else{
            incurrscopeoffset();
            rslt->sym = newtemp();
        }
        emit(uminus, $2, (expr_*)0, rslt, nextquadlabel(),yylineno);
        $$ = rslt;}
    
    | NOT expr		{
		expr_ * result;
		checkuminus($2);
				setScopeLine(scope,yylineno);

		result       =  newexpr(boolexpr_e);
		incurrscopeoffset();
		result->sym  =  newtemp();
		
		emit(not_, $2, NULL, result, nextquadlabel(),yylineno);
		$$ = result;}
    
    | DOUBLE_PLUS lvalue		{
		if(term_lvalue($2->id,scope)) 
		 yyerror("Cannot increment const function identifier");
		 incurrscopeoffset();
		 		setScopeLine(scope,yylineno);

		 lValue=newLvalue($2 ,yylineno,currscopespace() , currscopeoffset());
			$$=DOUBLE_PLUS_MINUS_lvalue(lValue, '+',yylineno);
    }
    | lvalue DOUBLE_PLUS{
		if(term_lvalue($1->id,scope))
		 yyerror("Cannot increment const function identifier");
		 incurrscopeoffset();
		 		setScopeLine(scope,yylineno);

			lValue=newLvalue($1 ,yylineno,currscopespace() , currscopeoffset());
			$$=lvalue_DOUBLE_PLUS_MINUS(lValue, '+',yylineno);
	}

    
    | DOUBLE_MINUS lvalue		{
		if(term_lvalue($2->id,scope))
		yyerror("Cannot decrement const function identifier");
		 incurrscopeoffset();
		setScopeLine(scope,yylineno);
		
					lValue=newLvalue($2 ,yylineno,currscopespace() , currscopeoffset());

			$$=DOUBLE_PLUS_MINUS_lvalue(lValue, '-',yylineno);

		}
    
    | lvalue DOUBLE_MINUS		{
		if(term_lvalue($1->id,scope))
			yyerror("Cannot decrement const function identifier");
		incurrscopeoffset();
		setScopeLine(scope,yylineno);
		
				lValue=newLvalue($1 ,yylineno,currscopespace() , currscopeoffset());

			$$=lvalue_DOUBLE_PLUS_MINUS(lValue, '-',yylineno);

		}
    
    | primary		{$$ = $1;}
   ;

assignexpr
    : lvalue ASSIGN expr{
			expr_*   rslt;
		setScopeLine(scope,yylineno);

			lValue=newLvalue($1 ,yylineno ,currscopespace() , currscopeoffset());
        if (lValue->type == tableitem_e ){
            emit(tablesetelem, lValue, lValue->index,$3, nextquadlabel(), yylineno);
            rslt        =   emit_iftableitem(lValue);
            rslt->type  =   var_e;
        }
        else{
            emit(assign, $3, NULL,$1->member, nextquadlabel(), yylineno);
            rslt = newexpr(var_e);
            incurrscopeoffset();
			rslt->sym = newtemp();
			//cout <<$1->member->sym->val.varVal->name<<" vvvvv\n";
            emit(assign,$1->member,NULL, rslt, nextquadlabel(), yylineno);
        }

				$$ = rslt;}
	;

primary
    : lvalue		{
		//incurrscopeoffset();
		lValue=newLvalue($1 ,yylineno , currscopespace() , currscopeoffset());
		setScopeLine(scope,yylineno);

		expr_	*res=emit_iftableitem(lValue);
			$$=res;}
    
    | call				{}
    
    | objectdef		{$$ = $1;}
    
    | OPEN_PARENTHESI funcdef CLOSE_PARENTHESI			{
		setScopeLine(scope,yylineno);

		$$= newexpr(programfunc_e);
 //       $$= (expr_*)$2;
	     $$->sym=$2;

		}
    
    | const			{$$ = $1;}
	;

lvalue
	:IDENT			{
		setScopeLine(scope,yylineno);
		 cout<<"bika\n";
	 expr_  *r	=lvalue_IDENT($1, yylineno,scope,funct,currscopeoffset()+1);
	 //		 //        incurrscopeoffset();

		lval *lVal=new lval();;
		lVal->id=$1;            
		lVal->type=SIMPLEID;
		lVal->member=r;
		$$=lVal;
			}
	|LOCAL_ IDENT   {
		setScopeLine(scope,yylineno);
		           
			  expr_  *r	=lvalue_LOCAL_IDENT($2, yylineno,scope,funct,currscopeoffset()+1);
	//incurrscopeoffset();
		lval *lVal=new lval();
		lVal->id=$2;
		lVal->type=LOCALID;
		lVal->member=r;
		$$=lVal;}
	|SCOPE IDENT	{
	           // incurrscopeoffset();
			  
				setScopeLine(scope,yylineno);
		expr_  *r	=lvalue_DOUBLE_COLON_IDENT($2, yylineno,scope,funct);
		lval *lVal=new lval();;
		lVal->id=$2;
		lVal->type=GLOBALID;
		lVal->member=r;
		$$=lVal;}
	|member {
	        //    incurrscopeoffset();
		lval *lVal=new lval();
		lVal->member=$1;
		lVal->type=MEMBER;
		$$ =lVal;
	}
	;

member
    : lvalue DOT IDENT		{
		//incurrscopeoffset();
				setScopeLine(scope,yylineno);
		
		lValue=newLvalue($1 ,yylineno ,currscopespace(),currscopeoffset()); 
		//expr_* item, *index;
		//lValue         = emit_iftableitem(lValue);
		//item         = newexpr(tableitem_e);
		//item->sym    =  lValue->sym;
		//index        =  newexpr_conststring($3);
		//item->index  =  index;
		//$$=item;
		

	$$=member_item(lValue,$3);
		}
    
    | lvalue OPEN_AGKILI expr CLOSE_AGKILI {
		setScopeLine(scope,yylineno);
	/*	expr_* tmpexpr =newexpr(boolexpr_e);
		tmpexpr->sym = (newtemp());
		emit(assign, newexpr_constbool(TRUE), NULL, tmpexpr, nextquadlabel(), yylineno);
		emit(jump, NULL, NULL, NULL, nextquadlabel() + 2, yylineno);
				emit(assign,newexpr_constbool(FALSE), NULL, tmpexpr, nextquadlabel(), yylineno);
	*/
		lValue=newLvalue($1 ,yylineno , currscopespace() , currscopeoffset());
		//incurrscopeoffset();
		/*if(!$3)
		{
		switch ( lValue->sym->scopeType ){
          case    FUNCTION    :$$=member_item(lValue,$3->sym->val.varVal->name);break;
                                                        case    VARIABLE    :
														  $$=member_item(lValue,$3->sym->val.varVal->name);
                                                            break;
                 
                                                }}*/
		
		expr_* item;
		lValue         =  emit_iftableitem(lValue);
		item         =  newexpr(tableitem_e);
		item->sym    =  lValue->sym;
		item->index  =$3;
		
		$$=item;
		}
    
    | call DOT IDENT	{
		setScopeLine(scope,yylineno);

		expr_* item, *index,*call;
        call         =  emit_iftableitem($1);
		item         =  newexpr(tableitem_e);
		item->sym    =  call->sym;
		index        =  newexpr_conststring($3);
		item->index  =  index;
		//$$=item;
		 $$ = member_item( $1, $3);
		}
    
    | call OPEN_AGKILI expr CLOSE_AGKILI	{
		expr_* item,*call, *tmpexpr;
		setScopeLine(scope,yylineno);
	    tmpexpr = newexpr(boolexpr_e);
		tmpexpr->sym = (newtemp());
		emit(assign, newexpr_constbool(TRUE), NULL, tmpexpr, nextquadlabel(), yylineno);
		emit(jump, NULL, NULL, NULL, nextquadlabel() + 2, yylineno);
		emit(assign, newexpr_constbool(FALSE), NULL, tmpexpr, nextquadlabel(), yylineno);
		call         =  emit_iftableitem($1);
		item         =  newexpr(tableitem_e);
		item->sym    =  call->sym;
		item->index  =  tmpexpr;
		$$=item;
/*		switch ( $3->sym->scopeType ){
                                                        case    FUNCTION    :
                                                            $$ = member_item( $1, $3->sym->val.funcVal->name);
                                                            break;
                                                        case    VARIABLE    :
                                                            $$ = member_item( $1, $3->sym->val.varVal->name);
                                                            break;
                                                        
                                                    }*/
		}
    ;

call
	//:call OPEN_PARENTHESI CLOSE_PARENTHESI
    //{
    //    list<expr_*>* tmp = new list<expr_*>;
   //     $$ = make_call( $1, (*tmp));
   // }

    :call OPEN_PARENTHESI elist CLOSE_PARENTHESI {
		setScopeLine(scope,yylineno);

	 $$ = make_call( $1, *$3);
	 
	 }
    
    |lvalue callsuffix	{
		list<expr_*>* tmp2 = new std::list<expr_*>;
		expr_* item, *index,*self;expr_ *lVal;
		//lValue=NULL;
		//incurrscopeoffset();
		
		lValue=newLvalue($1 ,yylineno , currscopespace() , currscopeoffset());
	/*	if ( $2->method ){

		    lVal=  emit_iftableitem(lValue);
			item=  newexpr(tableitem_e);
			item->sym    =  lVal->sym;
			index        =  newexpr_conststring($2->name);
			item->index  =  index;
			self=item;
			item= emit_iftableitem(item);

			if ((*($2->elist)).empty())		{
				$2->elist = new list<expr_*>;
                (*($2->elist)).push_back(self);
				}else{
				(*($2->elist)).push_back(self);
            }
            
				$$=make_call( item, (*($2->elist)));
			
			$$=lvalue_callsufix($2,lValue,self);
		}
		else{
			tmp2=$2->elist;
           $$ = make_call(item,*tmp2);
        }*/
		setScopeLine(scope,yylineno);

    if ( $2->method )
	{
            expr_*   self = lValue;
            lValue =emit_iftableitem( member_item(lValue, $2->name));
            if ((*($2->elist)).empty() ){
                $2->elist = new list<expr_*>;
                (*($2->elist)).push_back(self);
            }
            else{
                (*($2->elist)).push_back(self);
            }
            $$ = make_call( lValue, (*($2->elist)));
        }
        else{
					

            $$ = make_call( lValue, (*($2->elist)));
        }
		
		}
    
	

    |OPEN_PARENTHESI funcdef CLOSE_PARENTHESI OPEN_PARENTHESI elist CLOSE_PARENTHESI{
	//$$= make_call( (expr_*)$2,*$5);
		setScopeLine(scope,yylineno);

	expr_*   func    = newexpr(programfunc_e);
        func->sym       = $2;
        $$              = make_call( func, (*$5));

	
	}
	;

	
callsuffix
    : normcall		{$$ = $1;}
    
    | methodcall	{$$ = $1;}
    ;
    

normcall
	: OPEN_PARENTHESI elist CLOSE_PARENTHESI{

		std::list<expr_*>* tmp = new std::list<expr_*>;
		$$ = new callsuffix_();
        $$->elist   =$2;
        $$->method  = false;
        $$->name    = ( * new std::string(" ") );
		}
	;

	
methodcall
	: TWODOTS IDENT OPEN_PARENTHESI elist CLOSE_PARENTHESI	{
        $$ = new callsuffix_();
		$$->elist   =$4;
        $$->method  = true;
        $$->name    = $2;}
	;


elist
	: expr		{

		$$ = new list<expr_*>;
		$$->push_back($1);
		}
		
    | elist COMMA expr		{
		$$ = new list<expr_*>;
        $1->push_back($3);
        $$ =$1;
		}
    | {
		$$ = new std::list<expr_*>;
		}
    ;

    
objectdef
    : OPEN_AGKILI elist CLOSE_AGKILI{
		setScopeLine(scope,yylineno);

		expr_* t     = newexpr(newtable_e);
				incurrscopeoffset();

        t->sym= newtemp();

        emit( tablecreate, t,NULL, NULL, nextquadlabel(), yylineno );
        int i = 0;
        if ( !$2->empty() ){
            list<expr_*>::iterator it;
            for( it=$2->begin(); it!=$2->end(); ++it ){
                emit( tablesetelem, t, newexpr_constnum(i), (*it), nextquadlabel(), yylineno );
				i++;
			}
        }

        $$ = t;
		}
    
	 |OPEN_AGKILI  CLOSE_AGKILI 
    {
        //edo ftiaxnoume empty table, dld a = [];
        expr_* t     = newexpr(newtable_e);
         incurrscopeoffset();
        t->sym      = newtemp();
       
		emit( tablecreate, t, (expr_*)0, (expr_*)0, nextquadlabel(), yylineno );
        int i = 0;

        $$ = t;
    }

    |OPEN_AGKILI indexed CLOSE_AGKILI 	{
				setScopeLine(scope,yylineno);

		expr_* t    = newexpr(newtable_e);
		 incurrscopeoffset();
        t->sym      = newtemp();
		
        emit( tablecreate, (expr_*)0, (expr_*)0,t, nextquadlabel(), yylineno );
        if ( !($2)->empty() ){
            list<expr_*>::iterator it;
            for( it=$2->begin(); it!=$2->end(); ++it ){
                emit( tablesetelem, t, (*it)->index, (*it), nextquadlabel(), yylineno );
            }
        }
        $$ = t;
		}
    ;

indexed
    : indexedelem	{
		$$ = new std::list<expr_*>;
        $$->push_back($1);
		}
    
    | indexed COMMA indexedelem    {
		$$ = new list<expr_*>;
        $1->push_back($3);
        $$ =$1;
		}

    ;


indexedelem
    : OPEN_BRACKET expr COLON expr CLOSE_BRACKET		{$4->index = $2;
			$$ = $4;}
    ;

    
block
	:	OPEN_BRACKET
			{//enterscopespace();
			//resetfunctionlocalsoffset();
				scope++;funct++;
				 }
		stmts
		CLOSE_BRACKET
		{
			scope--;
			 Hide(scope);
			if ( funct )
            funct--;
			}
	;
	




funcname
    :IDENT{
        $$ = new std::string($1);
    }

    |{
		string *tmp;
		char* tmp2 = (char*) malloc (sizeof(char) * 5);
		sprintf(tmp2, "$f%d", cc++);
		tmp = ( new std::string(tmp2));
        $$ =tmp ;
    }
    ;


funcprefixAndArgs
    :FUNCTION_ funcname OPEN_PARENTHESI
    {
		loop--;
        funct++;
        ss.clear();
        functName = (*$2);
		  scopeoffsetstack.push( currscopeoffset() );
        enterscopespace();
        resetformalargsoffset();
    }
    idlist CLOSE_PARENTHESI
    {		setScopeLine(scope,yylineno);

		$$= FUNCTION_WITHOUT_IDENT(functName, ss,yylineno, scope,funct);
		    insertFormalArgs(ss,yylineno,scope);
    }
    ;

funcbody
    :block
    {
        $$ = currscopeoffset(); 
        exitscopespace(); 
		setScopeLine(scope,yylineno);
       // restorecurrscopeoffset(currscopeoffset());
    }
    ;

funcdef
    :funcprefixAndArgs funcbody
    {
		setScopeLine(scope,yylineno);
		exitscopespace();
		int oldoffset = scopeoffsetstack.top();
			cout<<oldoffset<<"dssdsfsdf\n";
        scopeoffsetstack.pop();
        restorecurrscopeoffset(oldoffset);
		//restorecurrscopeoffset(0);
	//	resetformalargsoffset();
        $1->val.funcVal->totalLocals = $2; 
        $$ = $1;   
        emit(funcend, lvalue_expr($1),NULL,NULL, nextquadlabel(), yylineno );
    }
    ;




const
    : INTEGER			{expr_* rslt = newexpr_constnum($1 );
                                $$ = rslt;}
    
    | DOUBLE			{expr_* rslt = newexpr_constnum( $1 );
                                $$ = rslt;}
    
    | STRING			{expr_* rslt = newexpr_conststring( *(new string($1)) );
                                $$ = rslt;
								}
    
    | NIL				{expr_* rslt = newexpr(nil_e);
                                $$ = rslt;}
    
    | TRUE			      {expr_* rslt = newexpr_constbool( true );
                                $$ = rslt;}
    
    | FALSE					{expr_* rslt = newexpr_constbool( false );
                                $$ = rslt;}
    |FLOAT 					{expr_* rslt = newexpr_constnum($1 );
                                $$ = rslt;}
   | ERROR		{yyerror("ERROR undefind character:\n");}
    ;


idlist
    : IDENT{
             
			setScopeLine(scope,yylineno);
			
			ss.push_back(new Function_Argument($1));
		$$=idlist_Ident($1,scope,yylineno);
		
		
	}
    | idlist COMMA IDENT   {
			setScopeLine(scope,yylineno);
			ss.push_back(new Function_Argument($3));
			$$=idlist_Comma_Ident($3,scope,yylineno);
		}
	|{ 			setScopeLine(scope,yylineno);}
    ;

ifprefix
    : IF OPEN_PARENTHESI expr CLOSE_PARENTHESI{
		$$ = nextquadlabel();
		emit(if_noteq, $3, newexpr_constbool(true), NULL, -1, yylineno);
	}
    ;
elseprefix
    : ELSE{
		$$ = nextquadlabel();
		emit(jump, NULL, NULL, NULL, -1, yylineno);
	}
    ;

ifstmt
    : ifprefix stmt	{
		setScopeLine(scope,yylineno);
		patchlabel($1, nextquadlabel() );
		$$ = $2;
	}
    
    | ifprefix stmt  elseprefix stmt{
		setScopeLine(scope,yylineno);
		patchlabel($1, $3+1);
		patchlabel($3, nextquadlabel() );
	}
	;

loopstart
    :{
		++loop;
		
	}
    ;
loopend
    :{
		--loop;

	}
    ;
loopstmt
    : loopstart stmt loopend{
		$$ = $2;
	}
    ;

whilestart
    : WHILE {
		$$ = nextquadlabel();
	}
    ;

whilecond
    : OPEN_PARENTHESI expr CLOSE_PARENTHESI	{
		emit(if_eq, $2, newexpr_constbool(TRUE), NULL, nextquadlabel()+2, yylineno);
		$$ = nextquadlabel();
		emit(jump, NULL, NULL, NULL, -1, yylineno);
		//loop=true;
	}	
	;

whilestmt
    : whilestart whilecond loopstmt	{
		setScopeLine(scope,yylineno);
		
		emit(jump, NULL, NULL, NULL, $1, yylineno);
		patchlabel( $2, nextquadlabel() );
		$3=statement;
		if ( $3 ){
            if ( !$3->breaklist.empty() ){
                patchlListlabels($3->breaklist, nextquadlabel());
            }
            if ( !$3->continuelist.empty() ){
                patchlListlabels($3->continuelist, $1);
            }
        }
	}
    ;

	N
	    :{
		$$ = nextquadlabel();
		emit(jump, NULL, NULL, NULL, -1, yylineno);
	}
    ;
M
    :{
		$$ = nextquadlabel();
	}
    ;

forprefix
    : FOR OPEN_PARENTHESI elist SEMICOLON M expr SEMICOLON		{
		setScopeLine(scope,yylineno);

		$$=(forprefix_ *)malloc( sizeof(forprefix_) );
		$$->test=$5;
		$$->enter= nextquadlabel();
		emit(if_eq, $6, newexpr_constbool(true), NULL, -1, yylineno);
	}
    ;


forstmt
    :forprefix N elist CLOSE_PARENTHESI N loopstmt N   {
		loop=true;
		currentLoop=NULL;
		setScopeLine(scope,yylineno);
		
		patchlabel($1->enter, $5+1);
		patchlabel($2, nextquadlabel());
		patchlabel($5, $1->test);
		patchlabel($7, $2+1);
		$6=statement;
		if ( $6 ){
            if ( !$6->breaklist.empty() ){
                patchlListlabels($6->breaklist, nextquadlabel());
            }
            if ( !$6->continuelist.empty() ){
                patchlListlabels($6->continuelist, $2+1);
            }
        }
	}
	;

returnstmt
    : RETURN SEMICOLON {
		emit(ret, NULL, NULL, NULL, nextquadlabel(), yylineno);
			
		}
    | RETURN expr SEMICOLON {emit(ret, $2, NULL, NULL, nextquadlabel(), yylineno);}
	;

	
%%
main(){
	initSymbolTable();
	yyparse();
	PrintSymbolTable();
	PrintQuads();
	generate();
        patch_incomplete_jump();
        writeAll();
}

int yyerror (string yaccProvidedMessage){
    cout<<yaccProvidedMessage<<  ": at line "<<  yylineno<<  ", before token:  "<<yytext<<".\n";
    cout<<  "INPUT NOT VALID\n";
}


expr_* equal(iopcode op, expr_* ex1, expr_* ex2, int sc, int ln, unsigned int off){
    expr_* rslt;
	switch(op){
    case if_eq :
        if
        (   (ex1->type == constnum_e || ex1->type == conststring_e || ex1->type == constbool_e || ex1->type == nil_e) &&
            (ex2->type == constnum_e || ex2->type == conststring_e || ex2->type == constbool_e || ex2->type == nil_e)
        )
        {
            if ( ex1->type == constnum_e && ex2->type == constnum_e ){
                if ( ex1->numConst == ex2->numConst ){
                    rslt = newexpr_constbool(true);
                }
                else{
                    rslt = newexpr_constbool(false);
                }
            }
            else if ( ex1->type == conststring_e && ex2->type == conststring_e ){
                if ( ex1->strConst.compare(ex2->strConst) == 0 ){
                    rslt = newexpr_constbool(true);
                }
                else{
                    rslt = newexpr_constbool(false);
                }
            }
            else if ( ex1->type == constbool_e && ex2->type == constbool_e ){
                if ( ex1->boolConst == ex2->boolConst ){
                    rslt = newexpr_constbool(true);
                }
                else{
                    rslt = newexpr_constbool(false);
                }
            }
            else if ( ex1->type == nil_e && ex2->type == nil_e ){
                rslt = newexpr_constbool(true);
            }
            else{
                rslt = newexpr_constbool(false);
            }
        }
        else{
            incurrscopeoffset();
            rslt = newexpr(boolexpr_e);
            rslt->sym = newtemp();

            emit(if_eq, ex1, ex2,NULL, nextquadlabel()+3, ln);
            emit(assign, newexpr_constbool(false), NULL, rslt, nextquadlabel(), ln);
            emit(jump, NULL,NULL,NULL, nextquadlabel()+2, ln);
            emit(assign, newexpr_constbool(true),NULL, rslt, nextquadlabel(), ln);
        }
     break;
        case if_noteq:
         if((ex1->type==constnum_e||ex1->type==conststring_e||ex1->type==constbool_e
         ||ex1->type==nil_e)&&(ex2->type==constnum_e||ex2->type==conststring_e||ex2->type
          ==constbool_e||ex2->type==nil_e)){
            if ( ex1->type == constnum_e && ex2->type == constnum_e ){
                if ( ex1->numConst != ex2->numConst ){
                   rslt = newexpr_constbool(true);
                }
                else{
                    rslt = newexpr_constbool(false);
                }
            }
            else if ( ex1->type == conststring_e && ex2->type == conststring_e ){
                if ( ex1->strConst.compare(ex2->strConst) != 0 ){
                    rslt = newexpr_constbool(true);
                }
                else{
                    rslt = newexpr_constbool(false);
                }
            }
            else if ( ex1->type == constbool_e && ex2->type == constbool_e ){
                if ( ex1->boolConst != ex2->boolConst ){
                    rslt = newexpr_constbool(true);
                }
                else{
                    rslt = newexpr_constbool(false);
                }
            }
            else if ( ex1->type == nil_e && ex2->type == nil_e ){
                rslt = newexpr_constbool(false);
            }
            else{
                rslt = newexpr_constbool(true);
            }
        }
        else{
            incurrscopeoffset();
            rslt = newexpr(boolexpr_e);
            rslt->sym = newtemp();
            emit(if_eq, ex1, ex2,NULL, nextquadlabel()+3, ln);
            emit(assign, newexpr_constbool(false),NULL, rslt, nextquadlabel(), ln);
            emit(jump,NULL,NULL,NULL, nextquadlabel()+2, ln);
            emit(assign, newexpr_constbool(true),NULL, rslt, nextquadlabel(), ln);
			}
		break;
	}

    return rslt;
}

