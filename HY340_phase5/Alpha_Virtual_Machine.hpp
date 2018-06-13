#ifndef ALPHA_VIRTUAL_MACHINE_HPP
    #include  <math.h>

#include "generate.hpp"
#define AVMCELLS_HPP_INCLUDED

#define         AVM_STACKSIZE       4096
#define         AVM_STACKENV_SIZE   4

#define     AVM_MAX_INSTRUCTIONS    (unsigned)nop_v
#define     AVM_NUMACTUALS_OFFSET   +4
#define     AVM_SAVEDPC_OFFSET      +3
#define     AVM_SAVEDTOP_OFFSET     +2
#define     AVM_SAVEDTOPSP_OFFSET   +1

#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic

#define     AVM_TABLE_HASHSIZE  211
#define AVM_WIPEOUT(m)  memset(&(m),0,sizeof(m))
#define         AVM_ENDING_PC       codeSize

#define         AVM_ENDING_PC       codeSize


enum avm_memcell_t{
    number_m = 0,
    string_m = 1,
    bool_m = 2,
    table_m = 3,
    userfunc_m = 4,
    libfunc_m = 5,
    nil_m = 6,
    undef_m = 7,
    var_m = 8
};

struct  avm_table;

struct  avm_memcell{
    avm_memcell_t   type;
    union data  {
        double      numVal;
        char*       strVal;
        bool        boolVal;
        avm_table*  tableVal;
        unsigned    funcVal;
        char*       libfuncVal;
    };
    data            cellData;
};
//avm_load
double consts_getnumber(unsigned index);
char * consts_getstring(unsigned index);
char * libfuncs_getused(unsigned index);
//avmStack
void avm_initstack(void);
avm_memcell* avm_translate_operand(vmarg *arg, avm_memcell *reg);
//avmTables



struct  avm_table_bucket{
    avm_memcell         key;
    avm_memcell         value;
};

typedef list<avm_table_bucket*> tableBucketList;

struct  avm_table{
    unsigned refCounter;
    tableBucketList strIndexed[AVM_TABLE_HASHSIZE];
    tableBucketList numIndexed[AVM_TABLE_HASHSIZE];
    unsigned total;
};


avm_table* avm_tablenew(void);
void avm_tabledestroy(avm_table* t);
avm_memcell* avm_tablegetelem(avm_table* t, avm_memcell* key);
void  avm_tablesetelem(avm_table* t, avm_memcell* key, avm_memcell* value);
void avm_tableincrrefcounter(avm_table* t);
void avm_tabledecrrefcounter(avm_table* t);
void avm_tablebucketsinit(tableBucketList* p);
void avm_tablebucketsdestroy(tableBucketList* p);
typedef void (*memclear_func_t)(avm_memcell*);
void avm_memcellclear(avm_memcell* m);
void memclear_string(avm_memcell* m);
void memclear_table(avm_memcell* m);


typedef void (*execute_func_t)(instruction *);

void execute_cycle(void);

void execute_assign(instruction* );
void execute_add(instruction* );
void execute_sub(instruction* );
void execute_mul(instruction* );
void execute_div(instruction* );
void execute_mod(instruction* );
void execute_jeq(instruction* );
void execute_jne(instruction* );
void execute_jle(instruction* );
void execute_jge(instruction* );
void execute_jlt(instruction* );
void execute_jgt(instruction* );
void execute_call(instruction* );
void execute_pusharg(instruction* );
void execute_funcenter(instruction* );
void execute_funcexit(instruction* );
void execute_newtable(instruction* );
void execute_tablegetelem(instruction* );
void execute_tablesetelem(instruction* );
void execute_jump(instruction* );
void execute_nop(instruction* );

void avm_error(const char *format);
void avm_dec_top(void);
void avm_push_envvalue(unsigned val);
void avm_callsaveenvironment(void);
unsigned avm_get_envvalue(unsigned i);
void avm_warning(const char *);
void avm_assign(avm_memcell *, avm_memcell *);
unsigned avm_totalactuals(void);
avm_memcell *avm_getactual(unsigned i);


typedef double (*arithmetic_func_t) (double x,double y);

double add_impl(double x, double y);
double sub_impl(double x, double y);
double mul_impl(double x, double y);
double div_impl(double x, double y);
double  mod_impl(double x, double y);

void execute_arithmetic(instruction* instr);

typedef void(*library_func_t)(void);

struct  libFuncs{
    string     name;
    library_func_t  func;
};

library_func_t  avm_getlibraryfunc(char* id);
void avm_calllibfunc(char *id);
void avm_registerlibfunc(const char *id, library_func_t addr);
userfunc* avm_getfuncinfo(unsigned   address);

typedef unsigned char (*tobool_func_t)(avm_memcell*);

unsigned char number_tobool(avm_memcell *m);
unsigned char string_tobool(avm_memcell *m);
unsigned char bool_tobool(avm_memcell *m);
unsigned char table_tobool(avm_memcell *m);
unsigned char userfunc_tobool(avm_memcell *m);
unsigned char libfunc_tobool(avm_memcell *m);
unsigned char nil_tobool(avm_memcell *m);
unsigned char undef_tobool(avm_memcell *m);
unsigned char avm_tobool(avm_memcell *m);

typedef char*   (*tostring_func_t)(avm_memcell*);

char* number_tostring(avm_memcell* m);
char* string_tostring(avm_memcell* m);
char* bool_tostring(avm_memcell* m);
char* table_tostring(avm_memcell* m);
char* userfunc_tostring(avm_memcell* m);
char* libfunc_tostring(avm_memcell* m);
char* nil_tostring(avm_memcell* m);
char* undef_tostring(avm_memcell* m);
char* avm_tostring(avm_memcell* m);

void libfunc_print(void);
void libfunc_typeof(void);
void libfunc_totalarguments(void);
void libfunc_argument(void);
void libfunc_objectmemberkeys(void); 
void libfunc_objecttotalmembers(void); 
void libfunc_objectcopy(void); 
void libfunc_cos(void);
void libfunc_sin(void);
void libfunc_input(void);
void libfunc_strtonum(void);
void libfunc_sqrt(void);


#endif
