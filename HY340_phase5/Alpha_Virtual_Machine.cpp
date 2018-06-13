#include "Alpha_Virtual_Machine.hpp"
#include "generate.hpp"

avm_memcell ax,bx,cx;
avm_memcell retval;
unsigned top,topsp;
avm_memcell stack_[AVM_STACKSIZE];


unsigned char executionFinished;
unsigned pc;
unsigned currLine;
unsigned codeSize;


instruction* code;
unsigned  totalActuals;
libFuncs* allLibFuncs [12];
unsigned  totalOfLibFuncs;

tostring_func_t tostringFuncs[]={
    number_tostring,
    string_tostring,
    bool_tostring,
    table_tostring,
    userfunc_tostring,
    libfunc_tostring,
    nil_tostring,
    undef_tostring
};
arithmetic_func_t arithmeticFuncs[]={
  add_impl,
  sub_impl,
  mul_impl,
  div_impl,
  mod_impl
};
tobool_func_t toboolFuncs[]={
  number_tobool,
  string_tobool,
  bool_tobool,
  table_tobool,
  userfunc_tobool,
  libfunc_tobool,
  nil_tobool,
  undef_tobool
};
const char* typeStrings[] ={
  "number",
  "string",
  "bool",
  "table",
  "userfunc",
  "libfunc",
  "nil",
  "undef"
};




double consts_getnumber(unsigned index){
	double*		numConsts_=get_numConsts();
	return numConsts_[index];
}

char* consts_getstring(unsigned index){
	extern char**  stringConsts;
		return stringConsts[index];
}

char* libfuncs_getused(unsigned index){
	extern char** namedLibfuncs;
	return namedLibfuncs[index];
}


//avmstack=============================================================
void avm_initstack(void){
    for ( unsigned i=0; i<AVM_STACKSIZE; ++i ){
         AVM_WIPEOUT( stack_[i]);
         stack_[i].type=undef_m;
    }
    top     = AVM_STACKSIZE-1;
    topsp   = 0;
}

avm_memcell* avm_translate_operand(vmarg *arg, avm_memcell *reg){
   
}
//avmtable=============================================================

avm_table* avm_tablenew(void){
    avm_table*  t 	=(avm_table*)malloc(sizeof(avm_table));
	AVM_WIPEOUT(*t);
    t->refCounter = t->total = 0;
    avm_tablebucketsinit(t->numIndexed);
    avm_tablebucketsinit(t->strIndexed);
	return t;
}

void avm_tabledestroy(avm_table* t){
    assert( t );
    avm_tablebucketsdestroy(t->numIndexed);
    avm_tablebucketsdestroy(t->strIndexed);
    delete t;
}

avm_memcell* avm_tablegetelem(avm_table* t, avm_memcell* key){

        
}
void avm_tableincrrefcounter(avm_table* t){
    ++t->refCounter;
}

void avm_tabledecrrefcounter(avm_table* t){
    assert( t->refCounter > 0 );
    if ( !--t->refCounter )
        avm_tabledestroy( t );
}

void avm_tablebucketsinit(tableBucketList* p){
    
}

void avm_tablebucketsdestroy(tableBucketList* p){
    
}

void avm_tablesetelem(avm_table* t, avm_memcell* key, avm_memcell* value){

}

memclear_func_t memclearFuncs[]={
     0, 
    memclear_string,
    0,
    memclear_table,
    0, 
    0, 
    0, 
    0 
};

void avm_memcellclear(avm_memcell *m){
    if(m->type != undef_m){
        memclear_func_t     f = memclearFuncs[m->type];
        if(f){
			(*f)(m);
        }
        m->type = undef_m;
    }
}

void memclear_string (avm_memcell *m){
    assert(m->cellData.strVal);
    free(m->cellData.strVal);
}

void memclear_table(avm_memcell *m){
    assert(m->cellData.tableVal);
    avm_tabledecrrefcounter(m->cellData.tableVal);
}
//Dispatcher=========================================================

execute_func_t executeFuncs[] = {
    execute_assign,
    execute_add,
    execute_sub,
    execute_mul,
    execute_div,
    execute_mod,   
    execute_jeq,
    execute_jne,
    execute_jle,
    execute_jge,
    execute_jlt,
    execute_jgt,
    execute_call,
    execute_pusharg,
    execute_funcenter,
    execute_funcexit,
    execute_newtable,
    execute_tablegetelem,
    execute_tablesetelem,
    execute_jump,
    execute_nop 
};

void execute_cycle(void){
    if(executionFinished){
        return;
        execute_cycle();
     }
    else if(pc == AVM_ENDING_PC){
        executionFinished = 1;
        return;
    }
    else {
        assert(pc < AVM_ENDING_PC);
        instruction *instr = code + pc;
        assert(instr->opcode >= 0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
        if(instr->scrLine){
            currLine = instr->scrLine;
        }
        unsigned oldPC = pc;
        (*executeFuncs[instr->opcode]) (instr);
        if(pc == oldPC){
            ++pc;
        }
    }
}

void execute_assign(instruction *instr){
    avm_memcell *lv = avm_translate_operand(&instr->result, (avm_memcell*) 0);
    avm_memcell *rv = avm_translate_operand(&instr->arg1, &ax);

    if ( lv ){
        assert(rv);

        avm_assign(lv, rv);
    }
    else{
        avm_assign(rv, &retval);
    }
}

void execute_call(instruction *instr){
    
}

void execute_pusharg (instruction *instr){
    
}

void execute_jeq(instruction* instr) {
    
}


void execute_jne(instruction *instr){
    
}

void execute_jle (instruction *instr){
    
}

void execute_jge(instruction *instr){
    
}

void execute_jlt (instruction *instr){
    
}

void execute_jgt (instruction *instr){
   
}

void execute_jump (instruction *instr){
   
}

void execute_nop(instruction *instr){
    
}

void execute_newtable(instruction *instr){
    avm_memcell* lv = avm_translate_operand (&instr->result, (avm_memcell*) 0);
    assert ( lv && (&stack_[0] <= lv && &stack_[top] > lv || lv == &retval) );

    avm_memcellclear(lv);

    lv->type = table_m;
    lv->cellData.tableVal = avm_tablenew();
    avm_tableincrrefcounter (lv->cellData.tableVal);
}

void execute_tablegetelem(instruction* instr){

   
}

void execute_tablesetelem(instruction *instr) {
    
}

void execute_funcenter(instruction *instr){
    
}

void execute_funcexit(instruction *unused) {
   
}

void avm_error(const char *format){
    
}

void avm_dec_top(void){
    
}

void avm_push_envvalue(unsigned val){
    stack_[top].type = number_m;
    stack_[top].cellData.numVal = val;
    avm_dec_top();
}

void avm_callsaveenvironment(void){
    avm_push_envvalue(totalActuals);
    avm_push_envvalue(pc+1);
    avm_push_envvalue(top + totalActuals + 2);
    avm_push_envvalue(topsp);
}

unsigned avm_get_envvalue(unsigned i) {
   //return val;
}

unsigned avm_totalactuals(void){ //the number of arguments "N"
  return    avm_get_envvalue(topsp+ AVM_NUMACTUALS_OFFSET);
}

avm_memcell*avm_getactual(unsigned i) {
    assert( i < avm_totalactuals()	);
    return &stack_[topsp + AVM_STACKENV_SIZE +1 + i];
}

void avm_warning(const char *format){
    
}

void avm_assign(avm_memcell *lv, avm_memcell *rv){
    

}

double add_impl	(double x, double y) {	return x+y; }
double sub_impl	(double x, double y) {	return x-y; }
double mul_impl	(double x, double y) {	return x*y; }

double div_impl(double x, double y) {
    if ( y!=0 )
        return x/y;
    else
        avm_error("Div with 0!!!\n");
}

double mod_impl(double x, double y) {
    if ( y!=0 )
        return ( (unsigned) x % (unsigned) y );
    else{
        avm_error("Mod with 0!!!\n");
    }
}

void execute_arithmetic(instruction* instr)	{
  
}
unsigned char number_tobool     (avm_memcell *m)	{   return m->cellData.numVal !=0 ;	    }
unsigned char string_tobool     (avm_memcell *m)   	{	return m->cellData.strVal[0] != 0;	}
unsigned char bool_tobool       (avm_memcell *m)	{	return m->cellData.boolVal;		    }
unsigned char table_tobool      (avm_memcell *m)	{	return 1;	                        }
unsigned char userfunc_tobool   (avm_memcell *m)	{	return 1;	                        }
unsigned char libfunc_tobool    (avm_memcell *m) 	{	return 1;	                        }
unsigned char nil_tobool        (avm_memcell *m)	{	return 0;	                        }
unsigned char undef_tobool      (avm_memcell *m)	{	assert(0); return 0;                }
unsigned char   avm_tobool  (avm_memcell *m) {
    assert(m->type >= 0 && m->type < undef_m);
    return (*toboolFuncs [m->type]) (m);
}
void avm_registerlibfunc (const char *id, library_func_t addr){
    
}

userfunc* avm_getfuncinfo(unsigned address){
    
    return  (userfunc*)0;
}

library_func_t  avm_getlibraryfunc(char* id){
   
}

void avm_calllibfunc(char *id){
    
}

char* avm_tostring(avm_memcell *m){
    
}

char* number_tostring(avm_memcell* m){
			return NULL;
}

char* string_tostring(avm_memcell* m){
    return  strdup(m->cellData.strVal);
}

char* bool_tostring(avm_memcell* m){
    return NULL;
}

char* table_tostring(avm_memcell* m){
    
    return NULL;
}

char* userfunc_tostring(avm_memcell* m){
   
    return NULL;
}

char* libfunc_tostring(avm_memcell* m){
    char* t = (char*) malloc (sizeof(char) * 2);
   

    return t;
}

char* nil_tostring(avm_memcell* m){
    char* t = (char*) malloc (sizeof(char) * 2);
    

    return t;
}

char* undef_tostring(avm_memcell* m){
    assert(false);
    char* t = (char*) malloc (sizeof(char) * 2);
   
    return t;
}

//LIBRARY_FUNCTIONS================================================================

void    libfunc_print(void) {
	
}

void    libfunc_typeof(void) {
 
}

void    libfunc_totalarguments(void) {
  
}

void    libfunc_cos(void){
    
}

void    libfunc_sin(void){
	
}
void libfunc_sqrt(void){
}

void libfunc_objectcopy(void){

}

void libfunc_argument(void){
}

void libfunc_objectmemberkeys(void){
    
}

void libfunc_objecttotalmembers(void){
 
}
void libfunc_input(void){
}

void libfunc_strtonum(void){
    
}
void avm_initialize(void){
  avm_initstack();

}



int main(void){
	
	 string a="alpha.abc";
    avm_load(a);
    writeAll();
	cout<<"hidrirri\n";
    avm_initialize();
    PrintSymbolTable();   
 while ( !executionFinished )
        execute_cycle();

    return 0;
}

