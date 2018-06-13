/* 
       Fall 2015
       Compilers
        Phase I
    Lexical Analyzer

    Author  :Hidri Klodjan
    Login  	:hidri
   lexical_analysis.l:		
        Edit Date:08/03/2011
*/

/*
 Hints for the project I take from this sites
		https://www.cs.princeton.edu/~appel/modern/c/software/flex/flex.html
		https://class.coursera.org/compilers-004/lecture

*/

%{
	
	#include "alphatoken.h"
%}


%option noyywrap
%option yylineno

/*Keywords*/
IFY						"if"
ELSEY					"else"
SWITCHY					"switch"
CASEY					"case"
WHILEY					"while"
FORY					"for"
DOY					"do"
FUNCTIONY				"function"
RETURNY					"return"
BREAKY					"break"
CONTINUEY				"continue"
ANDY					"and"
NOTY					"not"
ORY					"or"
TRUEY					"true"
FALSEY					"false"
LOCALY					"local"
NULLY					"NULL"


/*operators*/
DOUBLE_PLUSY				("++")
DOUBLE_MINUSY                   	("--")
ASSIGNY    				("=")
PLUSY					("+")
MINUSY					("-")
MULTIPLYY				("*")
DIVISIONY				("/")
PERSENTY				("%")
EQUALY					("==")
EQUAL_NOTY				("!=")

GREATERY				(">")
LESSY					("<")
GREATER_EQUALY                          (">=")
LESS_EQUALY				("<=")
AND1					("&")
OR1					("||")
NOT1					("!")

SCOPEY					("::")
twodotsY				("..")

/*constants*/
/*integer constant*/
DIGIT                                   [0-9]
IDY                                     [a-zA-Z]([a-zA-Z0-9]|_)*
INTCONSTY                               {DIGIT}+
/*real constant*/
DOUBLEY                                 ([[:digit:]]+"."[[:digit:]]+)
NEWLINE					("\n")
SPACE					(" ")
TAB 					("\t")
STRING					\"[^\n"]*\"

/*punctuation marks*/
OPEN_BRACKETY                           ("{")
CLOSE_BRACKETY                          ("}")
OPEN_AGKILIY				("[")
CLOSE_AGKILIY                           ("]")
OPEN_PARENTHESIY			("(")
CLOSE_PARENTHESIY                       (")")
SEMICOLONY				(";")
COMMAY					(",")
COLONY					(":")
DOTY					(".")
OPEN_COMMENTY                           ("/*")
CLOSE_COMMENTY                          ("*/")
LINE_COMMENTY                           ("//")
QUOTE                                   ("\"")
eat_stars                               ("*"+[^/])
eat_slashes                             ("/"+[^\*])
ERRORY                                  (.)

     
%x comment
%x commCpp
%x ileg
%x string
%%
================================================================================
{OPEN_COMMENTY}  {
	close_comment=0;
	open_comment=1;
        BEGIN(comment);
        tokenNumber++;
        tmp=yylineno;
}
<comment>[^\*\n/] {}
<comment>{OPEN_COMMENTY} {
	open_comment++;
	nestedline.push_back(tmp);
}
<comment>{CLOSE_COMMENTY}    {
                               	close_comment++;
				if(open_comment==close_comment){
					tokens_information.push_back( 
                                                               *new alpha_token(
                                                                tokenNumber,tmp,
                                                                ("\"\""),
                                                                ("COMMENT"),
                                                                ("C_STYLE_COMMENT") ));
                                     
                            
                                    for(k=0;k<nestedline.size();k++){
                                        tokens_information.push_back( *new alpha_token( 
                                        tokenNumber,nestedline[k],
                                                       (("\"\"")),
                                                    (("COMMENT")),
                                  (("NESTED_C_STYLE_COMMENT"))) );
                            }
                            nestedline.clear();
                            BEGIN(INITIAL);
					}
}
<comment>"\n"   {tmp++;}
<comment>eat_stars   {}
<comment>eat_slashes  {}
{LINE_COMMENTY}	{
		BEGIN(commCpp); tokenNumber++;tmp=yylineno;
		tokens_information.push_back( *new alpha_token(
                                    tokenNumber,
                                    tmp,
                                    ("\"\""),
                                    ("COMMENT"),
                                    ("CPP_STYLE_COMMENT") ));
      }
<commCpp>[^\\\n] {}
<commCpp>[\n] {BEGIN(INITIAL);}
<comment><<EOF>> {
		if(open_comment!=close_comment){
		tokens_information.push_back( *new alpha_token(
                                    tokenNumber,
                                    tmp,
                                    ("\"\""),
                                    ("ILLEGAL"),
                                    ("ILLEGAL_C_STYLE_COMMENT") ));
		}
		return -1;
		
}

{QUOTE} { tokenNumber++; BEGIN(string);str_line=yylineno;}
<string>"\n" {string_error = true;}
<string>[^\\\"]* {buffer_tmp +=yytext;} 
<string>"\\n" {buffer_tmp +="\n";}	
<string>"\\t" {buffer_tmp +="\t";}	
<string>"\\\"" {buffer_tmp +="\"";}     
<string>"\\\\" {buffer_tmp +="\\";}   
<string><<EOF>> {	
	cout<<str_line<<"  #ERROR: Unterminated STRING [ :"<<"  "<<buffer_tmp<<"]"<<endl;
         tokens_information.push_back( *new alpha_token(
                                                    tokenNumber,
                                                    str_line,
                                                    buffer_tmp,
                                                    "#ERROR UNTERMINATED STRING",
                                                    " "));
	return 0;
}
<string>"\\"[^nt\\<<EOF>>\"] {
        buffer_tmp +=yytext;
        illegal=true;
}
<string>"\"" {
	if (string_error) {						
		BEGIN(INITIAL);
                cout<<str_line<<" #ERROR: Multiline STRING: "<<buffer_tmp<<endl;
		string_error =false;
                
	}
	else{   
                if(illegal!=true){
                tokens_information.push_back( *new alpha_token(
                                                    tokenNumber,
                                                    str_line,
                                                    
                                                    buffer_tmp,
                                                    "STRING",
                                                    " "));}
                else
                tokens_information.push_back( *new alpha_token(
                                                    tokenNumber,
                                                    str_line,
                                                    buffer_tmp,
                                                    "ILLEGAL_STRING",
                                                    " "));
      	}
	BEGIN(INITIAL);
	yylval.stringValue = strdup(buffer_tmp.c_str() );	
        buffer_tmp.clear();
        illegal=false;
							

        return STRING;
}

{SPACE} {}
{TAB}	{}
{NEWLINE} {}
{IFY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("if"),
                                    ("KEYWORD"),
                                    ("IF") ));
	return  IF;
}
{ELSEY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("else"),
                                    ("KEYWORD"),
                                    ("ELSE") ));
                                     return ELSE;
	return  ELSE;
}
{WHILEY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("while"),
                                    ("KEYWORD"),
                                    ("WHILE") ));
                                     return WHILE;
	return  WHILE;
}

{FORY} {
	
	return  FOR;
}

{FUNCTIONY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("function"),
                                    ("KEYWORD"),
                                    ("FUNCTION") ));
                                     return FUNCTION_;
    
}

{RETURNY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("return"),
                                    ("KEYWORD"),
                                    ("RETURN") ));
	return  RETURN;
}

{BREAKY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("break"),
                                    ("KEYWORD"),
                                    ("break") ));
                                    return BREAK;
	
}

{CONTINUEY} {
	return  CONTINUE;
}

{ANDY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("and"),
                                    ("KEYWORD"),
                                    ("AND") ));
	return  AND;
}

{NOTY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("not"),
                                    ("KEYWORD"),
                                    ("NOT") ));
	return  NOT;
}

{ORY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("or"),
                                    ("KEYWORD"),
                                    ("OR") ));
	return  OR;
}

{LOCALY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("local"),
                                    ("KEYWORD"),
                                    ("LOCAL") ));
	return  LOCAL_;
}

{TRUEY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("true"),
                                    ("KEYWORD"),
                                    ("TRUE") ));
	return  TRUE;
}

{FALSEY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("false"),
                                    ("KEYWORD"),
                                    ("FALSE") ));
	return  FALSE;
}

{NULLY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("nil"),
                                    ("KEYWORD"),
                                    ("NULL") ));
	return  NIL;
}

{ASSIGNY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("="),
                                    ("OPERATOR"),
                                    ("ASSIGN") ));
                                    return ASSIGN;
	return  ASSIGN;
}

{PLUSY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("+"),
                                    ("OPERATOR"),
                                    ("PLUS") ));
	return  PLUS;
}

{MINUSY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("-"),
                                    ("OPERATOR"),
                                    ("MINUS") ));
	return  MINUS;
}

{MULTIPLYY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("*"),
                                    ("OPERATOR"),
                                    ("MULTIPLY") ));
                                    return MULTIPLY;
	return  MULTIPLY;
}

{DIVISIONY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("/"),
                                    ("OPERATOR"),
                                    ("DIVISION") ));
	return  DIVISION;
}

{PERSENTY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("%"),
                                    ("OPERATOR"),
                                    ("MOD") ));
	return  PERSENT;
}

{EQUALY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("=="),
                                    ("OPERATOR"),
                                    ("EQUAL") ));
	return  EQUAL;
}

{EQUAL_NOTY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("!="),
                                    ("OPERATOR"),
                                    ("EQUAL_NOT") ));
	return  EQUAL_NOT;
}

{DOUBLE_PLUSY} {
	return  DOUBLE_PLUS;
}

{DOUBLE_MINUSY} {
	return  DOUBLE_MINUS;
}

{GREATERY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (">"),
                                    ("OPERATOR"),
                                    ("GREATER") ));
	return  GREATER;
}

{LESSY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("<"),
                                    ("OPERATOR"),
                                    ("LESS") ));
	return  LESS;
}

{GREATER_EQUALY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (">="),
                                    ("OPERATOR"),
                                    ("GREATER_EQUAL") ));
	return  GREATER_EQUAL;
}

{LESS_EQUALY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("<="),
                                    ("OPERATOR"),
                                    ("LESS_EQUAL") ));
	return  LESS_EQUAL;
}

{OPEN_BRACKETY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("{"),
                                    ("PUNCTION"),
                                    ("OPEN_BRACKET") ));
	
	return  OPEN_BRACKET;
}

{CLOSE_BRACKETY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("}"),
                                    ("PUNCTION"),
                                    ("CLOSE_BRACKET") ));
	return  CLOSE_BRACKET;
}

{OPEN_AGKILIY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("["),
                                    ("PUNCTION"),
                                    ("OPEN_AGKILI	") ));
	return  OPEN_AGKILI;
}

{CLOSE_AGKILIY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("]"),
                                    ("PUNCTION"),
                                    ("CLOSE_AGKILI	") ));
	return  CLOSE_AGKILI;
}

{OPEN_PARENTHESIY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("("),
                                    ("PUNCTION"),
                                    ("OPEN_PARENTHESI") ));
	return  OPEN_PARENTHESI;
}

{CLOSE_PARENTHESIY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (""),
                                    ("PUNCTION"),
                                    ("CLOSE_PARENTHESI")));
	return  CLOSE_PARENTHESI;
}

{SEMICOLONY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (";"),
                                    ("PUNCTION"),
                                    ("SEMICOLON") ));
	return  SEMICOLON;
}

{COMMAY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (","),
                                    ("PUNCTION"),
                                    ("COMMA") ));
	return COMMA;
}

{COLONY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (":"),
                                    ("PUNCTION"),
                                    ("COLON")));
	return  COLON;
}

{SCOPEY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("::"),
                                    ("SIMIO STIKSIS"),
                                    ("SCOPE") ));
	return  SCOPE;
}

{DOTY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    ("."),
                                    ("PUNCTION"),
                                    ("DOT") ));
	return  DOT;
}

{twodotsY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (".."),
                                    ("SIMIO STIKSIS"),
                                    ("DOUBLE_DOT") ));
	return  TWODOTS;
}

{INTCONSTY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (yytext),
                                    ("INTCONT"),
                                    (yytext) ));
    yylval.intValue=(atoi(yytext));
	return  INTEGER;
}

{DOUBLEY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (yytext),
                                    ("REALCONST"),
                                    (yytext) ));
    yylval.realValue=(atof(yytext));
	return  DOUBLE;
}

{IDY} {
	tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (yytext),
                                    ("IDENT"),
                                    (yytext) ));
    yylval.stringValue = strdup(yytext);
	return  IDENT;
}
{ERRORY} { tokens_information.push_back( *new alpha_token(yylineno,
                                    tokenNumber++,
                                    (yytext),
                                    ("ERROR"),
                                    ("UNDEFIND TOKEN") ));
                                     yylval.stringValue = strdup(yytext);
                                    return ERROR;
	}
%%

