//Klodjan Hidri
#ifndef GENERATE_HPP_INCLUDED
#define GENERATE_HPP_INCLUDED
#include    	<string>
#include    	<list>
#include    	<stdlib.h>
#include    	"quads.hpp"
#include 	"def.hpp"
#include 	<fstream>

enum vmopcode{
     assign_v,		add_v,		sub_v,
     mul_v,	        div_v,	        mod_v,
     uminus_v,		and_v,		or_v,
     not_v,		    jeq_v,		jne_v,
     jle_v,		    jge_v,		jlt_v,
     jgt_v,		    call_v,		pusharg_v,
     funcenter_v,	    funcexit_v,		newtable_v,
     tablegetelem_v,	tablesetelem_v,		nop_v,
     jump_v
};

enum  vmarg_t{label_a     = 0,
                    global_a    = 1,
                    formal_a    = 2,
                    local_a     = 3,
                    number_a    = 4,
                    string_a    = 5,
                    bool_a      = 6,
                    nil_a       = 7,
                    userfunc_a  = 8,
                    libfunc_a   = 9,
                    retval_a    = 10,
                    nothing_a   = 11
                };

typedef struct  vmarg{
    vmarg_t     type;
    unsigned    val;
}vmarg;

void    generate_ADD(quad *);
void    generate_SUB(quad *);
void    generate_MUL(quad *);
void    generate_DIV(quad *);
void    generate_MOD(quad *);

void    generate_NEWTABLE(quad *);
void    generate_TABLEGETELEM(quad *);
void    generate_TABLESETELEM(quad *);
void    generate_ASSIGN(quad *);

void    generate_NOP(quad *);

void    generate_JUMP(quad *);
void    generate_IF_EQ(quad *);
void    generate_IF_NOTEQ(quad *);
void    generate_IF_GREATER(quad *);
void    generate_IF_GREATEREQ(quad *);
void    generate_IF_LESS(quad *);
void    generate_IF_LESSEQ(quad *);
void    generate_UMINUS(quad *);
void    generate_AND(quad *);
void    generate_NOT(quad *);
void    generate_OR(quad *);
void    generate_PARAM(quad *);
void    generate_CALL(quad *);
void    generate_GETRETVAL(quad *);
void    generate_FUNCSTART(quad *);
void    generate_RETURN(quad *);
void    generate_FUNCEND(quad *);
unsigned userfuncs_newfunc(Symbol * sym);
unsigned userfuncs_usedfunc(Symbol * sym);
void        writeAll (void);
void        writeMagicNumber(ofstream   *bin);
void        writeTotalGlobal(ofstream   *bin);
void        writeStringTable(ofstream    *bin, ofstream* readable);
void        writeNumberTable(ofstream    *bin, ofstream* readable);
void        writeUserFuncTable(ofstream    *bin, ofstream* readable);
void        writeLibFuncTable(ofstream    *bin, ofstream* readable);
void        writeInstructionTable(ofstream    *bin, ofstream* readable);

void Read_MagicNumber(ifstream *  bin);
void Read_Globals(ifstream *  bin);
void Read_StringsTable(ifstream *  bin);
void Read_NumbersTable(ifstream *  bin);
void Read_UserfunctionsTable(ifstream *  bin);
void Read_LibraryfunctionsTable(ifstream *  bin);
void Read_InstructionsTable (ifstream *  bin);
void avm_load(string);
void    backpatch   (void);

struct  instruction {
    vmopcode    opcode;
    vmarg       result;
    vmarg       arg1;
    vmarg       arg2;
    unsigned    scrLine;
};

#define EXPAND_SIZE_INS 1024
#define CURR_SIZE_INS   (total*sizeof(instruction))
#define NEW_SIZE_INS    (EXPAND_SIZE_INS*sizeof(instruction)+CURR_SIZE_INS)

struct userfunc {
  unsigned	    address;
  unsigned	    localise;
  string   id;
};


unsigned    consts_newstring(std::string s);
unsigned    consts_newnumber(double n);
unsigned    libfuncs_newused(string s);

void        make_numberoperand(vmarg* arg, double val);
void        make_booloperand(vmarg* arg, unsigned val);
void        make_retvaloperand(vmarg* arg);
void        make_operand(expr_* e, vmarg* arg);
void add_incomplete_jump(unsigned int, unsigned int);


	                                                    


struct incomplete_jump{
    unsigned    instrNo;    
    unsigned    iaddress;

};

void    patch_incomplete_jump( void );

void add_incomplete_jump(unsigned instrNo, unsigned iaddress);


typedef void ( *generator_func_t ) (quad*);

void    generate    (void);
void    generate    (vmopcode op, quad qd);

void    expandInst      (void);

void    emitInst        (instruction t);

unsigned    nextinstructionlabel   (void);
quad* get_quads();
instruction* get_instructions();
unsigned get_ij_total();
list<incomplete_jump*> get_ij_head();
list<userfunc*> get_userFuncs_();
unsigned get_totalUserFuncTableSlots();
unsigned get_totalUserFuncs();
userfunc *get_userFuncs();                          
char ** get_namedLibfuncs();
unsigned get_totalNamedLibfuncs();
double* get_numConsts();
unsigned get_totalNumConsts();
char ** get_stringConsts();
unsigned get_totalStringConsts();
#endif 
