//Klodjan Hidri

#include "generate.hpp"
#include <cerrno>
#include <string.h>
#include <stack>


#include    <string.h>
#include    <stdio.h>
#include    <stdlib.h>

#define     MAGIC_NUMBER    340200501



extern bool showDetails;
instruction* instructions ;
unsigned ij_total = 0;
const size_t N =100000;
vector<incomplete_jump*>ij_head(N);

stack<userfunctions*>funcstack;
size_t xx=0;
unsigned totalInst;
unsigned int currInst=0;
unsigned int currprocessedQuad=0;
double* numConsts;
unsigned totalNumConsts;
char** stringConsts;
unsigned totalStringConsts;
char** namedLibfuncs;
unsigned totalNamedLibfuncs;
userfunctions* funcstack_;
unsigned totalUserFuncs;

//e rrezikshme per kopjim
unsigned totalUserFuncTableSlots=0;
unsigned                    totalFuncTableSlots = 0;
unsigned                    totalStringTableSlots = 0;
unsigned                    totalNumTableSlots = 0;
unsigned	                totalNumConstsSize = 0;
unsigned	                totalStringConstsSize = 0;

//ifstream* readbinary_file;
//ofstream* binaryFile;
//ofstream* readableFile;


const    char* namedLibFunction[]={"print","input","objectmemberkeys",
                                                            "objecttotalmembers", "objectcopy",
                                                            "totalarguments", "argument", "typeof",
                                                            "strtonum", "sqrt", "cos", "sin"
                                                        };
unsigned int curr_Quad(){ return currprocessedQuad;}

quad * get_quads(){
	return getQuad();
}

instruction* get_instructions(){
	return instructions;
}
unsigned get_ij_total(){
	return ij_total;
}

vector<incomplete_jump*> get_ij_head(){
	return ij_head;
}

stack<userfunctions*> get_userFuncs_(){
	return funcstack;
}


unsigned get_totalUserFuncTableSlots(){
	return totalUserFuncTableSlots;
}

unsigned get_totalUserFuncs(){
	return totalUserFuncs;
}

userfunctions *get_userFuncs(){
	return funcstack_;
}
char ** get_namedLibfuncs(){
	return namedLibfuncs;
}

unsigned get_totalNamedLibfuncs(){
	return totalNamedLibfuncs;
	}
	                                                
double* get_numConsts(){
	return numConsts;
	}
unsigned get_totalNumConsts(){
	return totalNumConsts;
	}
char ** get_stringConsts(){
	return stringConsts;
}
unsigned get_totalStringConsts(){
	return totalStringConsts;
	}                                                      

unsigned    nextinstructionlabel (void){
    return currInst;
}

userfunctions* pop_(){
        userfunctions *a;
    if(!funcstack.empty()) {
        a=funcstack.top();
         funcstack.pop();
        return a;
    }
    else cout<<"stack is empty can't pop \n";
    return NULL;

}
void push_(userfunctions * a){
    funcstack.push(a);
}

userfunctions * top_(){
    userfunctions *a;
    Symbol *f;
    a=funcstack.top();
    return a;

}
void append_(list<unsigned int>a, unsigned int b){
    userfunctions *tm;
    a.push_back(b);
    tm=top_();
    tm->set_returnList(a);
}
//to dinei o savvidis
void expandInst(void){
    assert( totalInst==currInst );
    instruction* i = (instruction*)malloc(NEW_SIZE_INS);
    if ( instructions ){
        memcpy(i, instructions, CURR_SIZE_INS);
        free(instructions);
    }
    instructions = i;
    totalInst += EXPAND_SIZE_INS;
}

//to dinei o savvidis
void emit(instruction t){
    if ( currInst==totalInst )
        expandInst();
    instruction*   i   =   instructions+currInst++;
    i->arg1     =   t.arg1;
    i->arg2     =   t.arg2;
    i->result   =   t.result;
    i->scrLine  =   t.scrLine;
    i->opcode   =   t.opcode;

}
void make_operand(expr_* e, vmarg* arg){

    switch(e->type) {
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
        case newtable_e:{

            assert(e->sym);
            arg->val = e->sym->val.varVal->offset;

            switch (e->sym->symbolType) {
                case programvar: {
                    arg->type = global_a;
                    break;
                }
                case functionlocal:{
                    arg->type = local_a;break;
            }
                case formalarg: {
                    arg->type = formal_a;break;
                }
                default:
                   cout<<"Doesn't made operand in make_operand() from newtable_e\n";
                    assert(0);
            }
            break;
    }
        case constbool_e: {
            arg->val = e->boolConst;
            arg->type = bool_a;break;
        }
        case conststring_e: {
            arg->val = consts_newstring(e->strConst);
            arg->type = string_a;break;
        }
        case constnum_e: {
            arg->val = consts_newnumber(e->numConst);
            arg->type = number_a;break;
        }
        case nil_e: {
            arg->type = nil_a;break;
        }
        case programfunc_e: {
            arg->type = userfunc_a;
            arg->val = userfuncs_usedfunc(e->sym);break;
        }

        case libraryfunc_e: {
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(e->sym->val.funcVal->getName());break;
        }
        default: {
            cout<<"Doesn't made operand in make_operand() \n";
            assert(0);

        }
    }

	
}



//to dinei o savvidis
generator_func_t    generators[] = {
    generate_ASSIGN,
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_UMINUS,
    generate_AND,
    generate_OR,
    generate_NOT,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_IF_LESSEQ,
    generate_IF_GREATEREQ,
    generate_IF_LESS,
    generate_IF_GREATER,
    generate_CALL,
    generate_PARAM,
    generate_RETURN,
    generate_GETRETVAL,
    generate_FUNCSTART,
    generate_FUNCEND,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM,
    generate_JUMP,
    generate_NOP
};

//to dinei o savvidis
void generate(void){
    quad *qw=getQuad();
 unsigned	totals=nextquadlabel();
    for ( unsigned i=0; i<totals; i++ ){
        cout<<qw[0].line<<" gener\n";
        (*generators[qw[i].op-1])(qw);
    }
}

//to dinei o savvidis

void reset_operand (vmarg *arg){
    memset(arg,0,sizeof(arg));
}


void add_incomplete_jump(unsigned insNo, unsigned address){
    ij_head[xx]->iaddress=address;
    ij_head[xx]->instrNo=insNo;
    xx++;
	}


void generate(vmopcode op,quad *q){

    instruction t;
    t.opcode=op;
    if(q->arg1)
    make_operand(q->arg1,&t.arg1);
    if(q->arg2)
    make_operand(q->arg2,&t.arg2);
    if(q->result)
    make_operand(q->result,&t.result);
    q->taddress=nextinstructionlabel();
    emit(t);
}

//to dinei o savvidis
void generate_relational (vmopcode op, quad *qd){
    instruction t;
    t.opcode=op;
    make_operand(qd->arg1,&t.arg1);
    make_operand(qd->arg2,&t.arg2);
    t.result.type=label_a;
    if(qd->label==curr_Quad())
        t.result.val=qd[qd->label].taddress;
    else
        add_incomplete_jump(nextinstructionlabel(), qd->label);
        qd->taddress=nextinstructionlabel();
        emit(t);
}




void generate_PARAM(quad *qd){
    qd->taddress=nextinstructionlabel();
    instruction t;
    t.opcode=pusharg_v;
    make_operand(qd->arg1,&t.arg1);
    emit(t);

}

void generate_CALL(quad *qd){
    qd->taddress=nextinstructionlabel();
    instruction t;
    t.opcode=call_v;
    make_operand(qd->arg1,&t.arg1);
    emit(t);
}

void generate_GETRETVAL(quad *qd){
    qd->taddress=nextinstructionlabel();
    instruction t;
    t.opcode=assign_v;
    make_operand(qd->result,&t.result);
    make_retvaloperand(&t.arg1);
    emit(t);

}

void generate_UMINUS(quad *qd){
    
}

void generate_FUNCSTART(quad *qd){	

    Symbol *f;
    if(qd->result!=NULL)
    f=qd->result->sym;
    //f->getValue().funcVal->taddress =nextinstructionlabel();

    qd->taddress=nextinstructionlabel();

    if(f!=NULL)
    push_(new userfunctions(f->getValue().funcVal->getName(),f->getValue().funcVal->taddress,
                              f->getValue().funcVal->get_totallocals()));

    instruction t;
    t.opcode=funcenter_v;
   if(qd->result)
    make_operand(qd->result,&t.result);
    emit(t);
}

void generate_RETURN(quad *qd){
    userfunctions *a;
    qd->taddress=nextinstructionlabel();
    instruction t;
    t.opcode=assign_v;
    make_retvaloperand(&t.result);
    make_operand(qd->arg1,&t.arg1);
    emit(t);
    a=top_();
    append_(a->get_return_List(),nextinstructionlabel());
    t.opcode=jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type=label_a;
    emit(t);

}

void generate_FUNCEND(quad *qd){
    userfunctions *a;
    a=pop_();
    backpatch(a->get_return_List(),nextinstructionlabel());
    qd->taddress=nextinstructionlabel();
    instruction t;
    t.opcode=funcexit_v;
    make_operand(qd->result,&t.result);
    emit(t);
}

void    backpatch   (list<unsigned int>a, unsigned int b){

    for (int i : a){
        instructions[i].result.val=b;
        instructions[i].result.type=label_a;
    }

}

void    generate_NOT(quad *qd){
    qd->taddress=nextinstructionlabel();
    instruction t;
    t.opcode=jeq_v;
    make_operand(qd->arg1,&t.arg1);
    make_booloperand(&t.arg2,false);
    t.result.type=label_a;
    t.result.val=nextinstructionlabel()+3;

    t.opcode=assign_v;
    make_booloperand(&t.arg1,false);
    reset_operand(&t.arg2);
    make_operand(qd->result,&t.result);
    emit(t);

    t.opcode=jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type=label_a;
    t.result.val=nextinstructionlabel()+2;
    emit(t);

    t.opcode=assign_v;
    make_booloperand(&t.arg1,true);
    reset_operand(&t.arg2);
    make_operand(qd->result,&t.result);
    emit(t);

}
void    generate_OR(quad *qd) {

    instruction t;

    qd->taddress  =  nextinstructionlabel();
    t.opcode        =  jeq_v;

    make_operand(qd->arg1, &t.arg1);
    make_booloperand(&t.arg2, true);
    t.result.type   =  label_a;
    t.result.val    =  nextinstructionlabel()+4;
    emit(t);

    make_operand(qd->arg2, &t.arg1);
    t.result.val    =  nextinstructionlabel()+3;
    emit(t);

    t.opcode  =  assign_v;
    make_booloperand(&t.arg1, false);
    reset_operand(&t.arg2);
    make_operand(qd->result, &t.result);
    emit(t);

    t.opcode=jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type=label_a;
    t.result.val=nextinstructionlabel()+2;
    emit(t);

    t.opcode=assign_v;
    make_booloperand(&t.arg1, true);
    reset_operand(&t.arg2);
    make_operand(qd->result, &t.result);
    emit(t);
}

void    generate_AND(quad *qd){
    instruction t;

    qd->taddress  =  nextinstructionlabel();
    t.opcode        =  jeq_v;

    make_operand(qd->arg1, &t.arg1);
    make_booloperand(&t.arg2, true);
    t.result.type   =  label_a;
    t.result.val    =  nextinstructionlabel()+4;
    emit(t);

    make_operand(qd->arg2, &t.arg1);
    t.result.val    =  nextinstructionlabel()+3;
    emit(t);

    t.opcode  =  assign_v;
    make_booloperand(&t.arg1, false);
    reset_operand(&t.arg2);
    make_operand(qd->result, &t.result);
    emit(t);

    t.opcode=jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type=label_a;
    t.result.val=nextinstructionlabel()+2;
    emit(t);

    t.opcode=assign_v;
    make_booloperand(&t.arg1, true);
    reset_operand(&t.arg2);
    make_operand(qd->result, &t.result);
    emit(t);

}
void    generate_ADD(quad *qd){         generate(add_v,qd); }
void    generate_SUB(quad *qd){         generate(sub_v,qd); }
void    generate_MUL(quad *qd){         generate(mul_v,qd); }
void    generate_DIV(quad *qd){         generate(div_v,qd); }
void    generate_MOD(quad *qd){         generate(mod_v,qd); }
void    generate_NEWTABLE(quad *qd)	{   generate(newtable_v,qd); }
void    generate_TABLEGETELEM(quad *qd){generate(tablegetelem_v,qd); }
void    generate_TABLESETELEM(quad *qd){generate(tablesetelem_v,qd); }
void    generate_ASSIGN(quad *qd)	{   generate(assign_v,qd); };
void    generate_JUMP(quad *qd)	{       generate_relational(jump_v,qd); }
void    generate_IF_EQ(quad *qd)	{   generate_relational(jeq_v,qd); }
void    generate_IF_NOTEQ(quad *qd)	{   generate_relational(jne_v,qd); }
void    generate_IF_GREATER(quad *qd){  generate_relational(jgt_v,qd); }
void    generate_IF_GREATEREQ(quad *qd){generate_relational(jge_v,qd); }
void    generate_IF_LESS(quad *qd){     generate_relational(jlt_v,qd); }
void    generate_IF_LESSEQ(quad *qd){   generate_relational(jle_v,qd); }
void    generate_NOP(quad *qd)	{ instruction t = (*new instruction()); t.opcode = nop_v; emit(t);}


void    patch_incomplete_jump   ( void ){
    for ( int i=0;i<ij_head.size();i++){
        if ( ij_head[i]->iaddress == CURR_SIZE ){
            instructions[ij_head[i]->instrNo].result.val = CURR_SIZE_INS;
            //instructions[ij_head[i]->instrNo].result.type = label_a;
        }
        else{
            instructions[ij_head[i]->instrNo].result.val = get_quads()[ij_head[i]->iaddress].taddress;
            //instructions[ij_head[i]->instrNo].result.type = label_a;
        }
    }
}

//to dinei o savvidis
unsigned    consts_newstring    (std::string s){


    return (unsigned)atoi( s.c_str() );
}
//to dinei o savvidis
unsigned    consts_newnumber    (double n){
    bool        foundSameNumber = false;
    unsigned    i;

    for ( i=0; i<totalNumTableSlots; ++i ){
        if ( numConsts[i] == n ){
            foundSameNumber = true;
            break;
        }
    }

    if ( foundSameNumber )
        return i;
    else{
        //expand to newsize = currsize + 1024doubles more if necessary
        if ( totalNumTableSlots == totalNumConstsSize ){
            double* tmp = (double*)malloc( 1024*sizeof(double) + (totalNumConstsSize*sizeof(double)) );
            assert( tmp );

            if ( numConsts ){
                memcpy( tmp, numConsts, (totalNumConstsSize*sizeof(double)) );
                free(numConsts);
            }

            numConsts = tmp;
            totalNumConstsSize += 1024;
        }
        //now we add the number to the table
        double*     toAdd   =   numConsts+totalNumTableSlots++;
        (*toAdd)            =   n;

        return totalNumTableSlots-1; //giati exoume kanei ++ kai zitame index pinaka
    }
}

 unsigned userfuncs_newfunc(Symbol * sym){}

unsigned userfuncs_usedfunc(Symbol * f) {}


unsigned    libfuncs_newused    (std::string s){
    return (unsigned)atoi( s.c_str() );
}


//to dinei o savvidis
void        make_numberoperand  (vmarg* arg, double val)
{
    arg->val = consts_newnumber(val);
    arg->type = number_a;
   
}
//to dinei o savvidis
void        make_booloperand    (vmarg* arg, unsigned val){
    arg->type = bool_a;
    arg->val  = val;
}
//to dinei o savvidis
void        make_retvaloperand  (vmarg* arg){
    arg->type = retval_a;
}







//==================================shume kod i rrezikshem===============================================

string     vmopcodeToString        (vmopcode opcode){
    cout<<opcode<<"sto vmstring\n";
    switch ( opcode ){
        case    assign_v            :   return (* new std::string("assign"));
        case    add_v               :   return (* new std::string("add"));
        case    sub_v               :   return (* new std::string("sub"));
        case    mul_v               :   return (* new std::string("mul"));
        case    div_v               :   return (* new std::string("div"));
        case    mod_v               :   return (* new std::string("mod"));
        case    uminus_v            :   return (* new std::string("uminus"));
        case    and_v               :   return (* new std::string("and"));
        case    or_v                :   return (* new std::string("or"));
        case    not_v               :   return (* new std::string("not"));
        case    jeq_v               :   return (* new std::string("jeq"));
        case    jne_v               :   return (* new std::string("jne"));
        case    jle_v               :   return (* new std::string("jle"));
        case    jge_v               :   return (* new std::string("jge"));
        case    jlt_v               :   return (* new std::string("jlt"));
        case    jgt_v               :   return (* new std::string("jgt"));
        case    call_v              :   return (* new std::string("call"));
        case    pusharg_v           :   return (* new std::string("pusharg"));
        case    funcenter_v         :   return (* new std::string("funcenter"));
        case    funcexit_v          :   return (* new std::string("funcexit"));
        case    newtable_v          :   return (* new std::string("newtable"));
        case    tablegetelem_v      :   return (* new std::string("tablegetelem"));
        case    tablesetelem_v      :   return (* new std::string("tablesetelem"));
        case    nop_v               :   return (* new std::string("nop"));
        case    jump_v              :   return (* new std::string("jump"));
        default                     :   assert( false );
    }
}

std::string     vmargTypeToString       (vmarg_t tp){
    cout<<tp <<" vmarg --\n";
    std::string str;
    switch (tp){
        case 0:
            str="00 (label)";
            break;
        case 1:
            str="01 (global)";
            break;
        case 2:
            str="02 (formal)";
            break;
        case 3:
            str="03 (local)";
            break;
        case 4:
            str="04 (number)";
            break;
        case 5:
            str="05 (string)";
            break;
        case 6:
            str="06 (bool)";
            break;
        case 7:
            str="07 (nil)";
            break;
        case 8:
            str="08 (userfunc)";
            break;
        case 9:
            str="09 (libfunc)";
            break;
        case 10:
            str="10 (retval)";
            break;
        case 11:
            str="";
            break;
        default:cout<<"lathos\n";
           // assert(false);
    }
    return str;
}


FILE*                   binaryFile;
std::ofstream*          readableFile;
/*void writeAll(void){
 const char* s="alpha.abc";
 
}

void        writeMagicNumber            (std::ofstream  *bin){
	 }

void  writeStringTable        ( ofstream *bin,ofstream *readableFile ){

}


void        writeNumberTable            (ofstream  *  bin,ofstream* readable){

	}
void        writeUserFuncTable          (ofstream   * bin,ofstream* readable){
	 

	}
void        writeLibFuncTable           (ofstream    *bin,ofstream* readable){

	}
void writeInstructionTable(ofstream *bin,ofstream* readable){
	
	}

void Read_MagicNumber(ifstream *  bin){
	}
void Read_Globals(ifstream *  bin){

	}
void Read_StringsTable(ifstream *  bin){
	
	}
void Read_NumbersTable(ifstream *  bin){
	}
void Read_UserfunctionsTable(ifstream *  bin){
}
void Read_LibraryfunctionsTable(ifstream *  bin){
	
}
void Read_InstructionsTable (ifstream *  bin){
	}
*/

void writeAll(void){
    cout<<"u futa re \n";
//=======================
// binary file
//=======================
    binaryFile      =   fopen("/home/klodjan/Dropbox/klodjan_compilers/HY340_phase5/alpha.abc", "w");
    if ( !binaryFile ){
        std::cerr   <<  "Couldn't create binary file !!!!\nSo, terminating...\n";
        //assert( false );
    }

//=======================
// readable file
//=======================
    readableFile = new std::ofstream("/home/klodjan/Dropbox/klodjan_compilers/HY340_phase5/readable.txt");
    if ( !readableFile ){
        std::cerr   <<  "Couldn't create readable file !!!!\nSo, terminating...\n";
       // assert( false );
    }
//=======================
//call all "write" functions
//=======================
    writeMagicNumber        ( binaryFile );
    writeTotalGlobal        ( binaryFile );
    writeStringTable        ( binaryFile, readableFile );
    writeNumberTable        ( binaryFile, readableFile );
    writeUserFuncTable      ( binaryFile, readableFile );
    writeLibFuncTable       ( binaryFile, readableFile );
    writeInstructionTable   ( binaryFile, readableFile );

    fclose(binaryFile);
    (*readableFile).close();
}

void    writeMagicNumber            (FILE*   bin){
    assert( bin );
    unsigned    mnumber = MAGIC_NUMBER;
    fwrite( &mnumber, sizeof(unsigned), 1, bin );
}

void    writeTotalGlobal            (FILE*   bin ){
    assert( bin );
    unsigned u=get_progVar();
    fwrite( &u, sizeof(unsigned), 1, bin );
}

void    writeStringTable            (FILE* bin, std::ofstream* readable){
    unsigned i , length;
    *readable   <<  "STRING_TABLE\n"
                <<  "_________________________\n";

    fwrite(&totalStringTableSlots, sizeof(unsigned), 1, bin);
    for(i=0; i<totalStringTableSlots; i++){
        *readable   << "\t" <<   i <<"|\t"  <<stringConsts[i] <<"\n";
        length = strlen(stringConsts[i])+1;
        fwrite( &length, sizeof(unsigned), 1, bin );
        fwrite( stringConsts[i], length, 1, bin );
    }

    *readable <<    "\n\n";
}

void    writeNumberTable            (FILE* bin, std::ofstream* readable){
    unsigned i;
    *readable   <<  "NUMBER_TABLE\n"
                <<  "_________________________\n";

    fwrite(&totalNumTableSlots, sizeof(unsigned), 1, bin);
    for(i=0; i < totalNumTableSlots;  i++)
    {cout<<"bka for\n";
        *readable   << "\t" << std::left  << std::setw(3) << i
                    <<"|\t" <<numConsts[i] <<"\n";
        fwrite( &numConsts[i], sizeof(double), 1, bin);
    }

    *readable <<"\n\n";
}

void    writeUserFuncTable (FILE*   bin, std::ofstream* readable){
    unsigned    length;
    unsigned    index = 0;
    *readable   <<  "USER_FUNCTION_TABLE <id,addr,lcls>\n"
                <<  "___________________________________\n";

    fwrite(&totalFuncTableSlots, sizeof(unsigned), 1, bin);
    stack<userfunctions *>it ;
    it=funcstack;
    userfunctions *a;

    while (!funcstack.empty() ){
        index++;
        a=pop_();
        *readable  <<  "\t"    << std::left  << std::setw(3) << index
                   <<  "|"
                   <<  std::left << std::setw(10) << a->get_id()
                   <<  " |"
                   <<  std::left << std::setw(3) << a->get_taddress()
                   <<  " |"
                   <<  std::left << std::setw(3) << a->get_totalcalls()
                   <<  "\n";
        length = strlen(a->get_id().c_str()) +1;

        //fwrite((a->get_taddress()), sizeof(unsigned),1,bin);
        //fwrite((a->get_totalcalls()), sizeof(unsigned),1,bin);
        //fwrite(&length, sizeof(unsigned),1,bin);
        //fwrite((a->get_id()), sizeof(char),length,bin);
    }

    *readable << "\n\n";
}

void writeLibFuncTable(FILE*   bin, std::ofstream* readable){
    *readable   <<  "LIBRARY_FUNCTION_TABLE\n"
                <<  "_________________________\n";

    unsigned    length;
    unsigned    tableSize = 12;

    fwrite( &tableSize, sizeof(unsigned), 1, bin );
    for ( int i=0; i<tableSize; ++i ){
        *readable   <<  "\t"    << std::left  << std::setw(3) << i
                    <<  "|\t"   << namedLibFunction[i]
                    <<  "\n";

        length = strlen(namedLibFunction[i]) + 1; //null terminated!!
        fwrite( &length, sizeof(unsigned), 1, bin );
        fwrite( namedLibFunction[i], length, 1, bin );
    }

    *readable   <<  "\n\n";
}

void    writeInstructionTable       (FILE*   bin, std::ofstream* readable){
    *readable   <<  "INSTRUCTION_TABLE\n"
                <<  "_________________________\n\n\t"
                <<  std::left
                <<  std::setw(13)   <<  "INDEX"
                <<  std::setw(13)   <<  "OPCODE"
                <<  std::setw(13)   <<  "ARG_1_TYPE"
                <<  std::setw(13)   <<  "ARG_1_VALUE"
                <<  std::setw(13)   <<  "ARG_2_TYPE"
                <<  std::setw(13)   <<  "ARG_2_VALUE"
                <<  std::setw(13)   <<  "RESULT_TYPE"
                <<  std::setw(13)   <<  "RESULT_VALUE"
                <<  std::endl
                <<  "\t____________________________________________________________________________________________________________\n\n";

    for(unsigned i = 0; i < currInst; i++){
        *readable   <<  "\t"
                    <<  std::left
                    <<  std::setw(13)   << i
                    <<  std::setw(13)   << vmopcodeToString(instructions[i].opcode)
                    <<  std::setw(13)   << vmargTypeToString(instructions[i].arg1.type);

        if ( instructions[i].arg1.type != nothing_a )
            *readable   <<  std::setw(13)   << instructions[i].arg1.val;
        else
            *readable   <<  std::setw(13)   << "";

        *readable   <<  std::setw(13)   << vmargTypeToString(instructions[i].arg2.type);

        if ( instructions[i].arg2.type != nothing_a )
            *readable   <<  std::setw(13)   << instructions[i].arg2.val;
        else
            *readable   <<  std::setw(13)   << "";

        *readable   <<  std::setw(13)   << vmargTypeToString(instructions[i].result.type);

        if ( instructions[i].result.type != nothing_a ){
            if ( instructions[i].result.val != -1 )
                *readable   <<  std::setw(13)   << instructions[i].result.val;
            else
                *readable   <<  std::setw(13)   << "";
        }
        else
            *readable   <<  std::setw(13)   << "";

        *readable   <<  "\n"
                    <<  "\t____________________________________________________________________________________________________________\n\n";
    }
    fwrite(&currInst, sizeof(unsigned), 1, bin);
    //save in this order : vmopcode   result  arg1  arg2  line
    fwrite(instructions, sizeof(instruction), currInst, bin);

    *readable << "\n\n";
}

void avm_load( string filename){

}


//Klodjan Hidri
/*
#include "generate.hpp"
#include <cerrno>
#include <string.h>           
extern bool    showDetails;
instruction*    instructions ;

unsigned ij_total = 0;
                 list<incomplete_jump*>ij_head; 
extern unsigned    total;  
extern  quad*       quads; 
              list<userfunc*>userFuncs_;

unsigned            totalInst;
unsigned int        currInst    =   0;
unsigned int	    currprocessedQuad = 0;

double*		    numConsts;          
unsigned	    totalNumConsts;

char **    		stringConsts;       
unsigned	    totalStringConsts;

char**			namedLibfuncs;      
unsigned	    totalNamedLibfuncs;

userfunc*	    userFuncs;         
unsigned	    totalUserFuncs;
unsigned		totalUserFuncTableSlots=0;
//list<userfunc*>	    userFuncs_;    
ifstream    *      readbinary_file;

ofstream  *       binaryFile;
ofstream*          readableFile;


const    char* namedLibFunction[]={"print","input","objectmemberkeys",
                                                            "objecttotalmembers", "objectcopy",
                                                            "totalarguments", "argument", "typeof",
                                                            "strtonum", "sqrt", "cos", "sin"
                                                        };


quad * get_quads(){
	return quads;
}

instruction* get_instructions(){
	return instructions;
}
unsigned get_ij_total(){
	return ij_total;
}

list<incomplete_jump*> get_ij_head(){
	return ij_head;
}

list<userfunc*> get_userFuncs_(){
	return userFuncs_;
}

unsigned get_totalUserFuncTableSlots(){
	return totalUserFuncTableSlots;
}

unsigned get_totalUserFuncs(){
	return totalUserFuncs;
}

userfunc *get_userFuncs(){
	return userFuncs;
}
char ** get_namedLibfuncs(){
	return namedLibfuncs;
}

unsigned get_totalNamedLibfuncs(){
	return totalNamedLibfuncs;
	}
	                                                
double* get_numConsts(){
	return numConsts;
	}
unsigned get_totalNumConsts(){
	return totalNumConsts;
	}
char ** get_stringConsts(){
	return stringConsts;
}
unsigned get_totalStringConsts(){
	return totalStringConsts;
	}                                                      

unsigned    nextinstructionlabel (void){
    return currInst;
}

//to dinei o savvidis
void    expandInst (void){
    assert( totalInst==currInst );

    instruction* i = (instruction*)malloc(NEW_SIZE_INS);
    if ( instructions ){
        memcpy(i, instructions, CURR_SIZE_INS);
        free(instructions);
    }

    instructions = i;
    totalInst += EXPAND_SIZE_INS;
}

//to dinei o savvidis
void emitInst(instruction t){
    if ( currInst==totalInst )
        expandInst();

    instruction*   i   =   instructions+currInst++;
    i->arg1     =   t.arg1;
    i->arg2     =   t.arg2;
    i->result   =   t.result;
    i->scrLine  =   t.scrLine;
    i->opcode   =   t.opcode;

}
void        make_operand        (expr_* e, vmarg* arg){
	
}



//to dinei o savvidis
generator_func_t    generators[] = {
    generate_ASSIGN,
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_UMINUS,
    generate_AND,
    generate_OR,
    generate_NOT,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_IF_LESSEQ,
    generate_IF_GREATEREQ,
    generate_IF_LESS,
    generate_IF_GREATER,
    generate_CALL,
    generate_PARAM,
    generate_RETURN,
    generate_GETRETVAL,
    generate_FUNCSTART,
    generate_FUNCEND,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM,
    generate_JUMP,
    generate_NOP
};

//to dinei o savvidis
void    generate    (void){
 unsigned	totals=nextquadlabel();
	quad *quads1=getQuad();
    for ( unsigned i=0; i<totals; i++ ){
			
        (*generators[quads1[i].op])(quads1);
    }
}

//to dinei o savvidis

void reset_operand (vmarg *arg){
    memset(arg,0,sizeof(arg));
}
//to dinei o savvidis
void    generate_instruction (vmopcode op, quad *qd){
  
instruction t = (*new instruction());

    t.opcode = op;

    if ( qd->arg1 )
        make_operand(qd->arg1, &t.arg1);

    if ( qd->arg2 )
        make_operand(qd->arg2, &t.arg2);

    if ( qd->result )
        make_operand(qd->result, &t.result);

    qd->taddress = nextinstructionlabel();
    emitInst(t);
    
}

void add_incomplete_jump(unsigned insNo, unsigned address){

	}

//to dinei o savvidis
void generate_relational (vmopcode op, quad *qd){
   
}

void generate_NOT(quad *qd){
    
}

void generate_OR(quad *qd){
    
}

void generate_AND(quad *qd){
    
}

void generate_PARAM(quad *qd){
}

void generate_CALL(quad *qd){
    
}

void generate_GETRETVAL(quad *qd){
   
}

void generate_UMINUS(quad *qd){
    
}

void generate_FUNCSTART(quad *qd){	
  
}

void generate_RETURN(quad *qd){
   
}

void generate_FUNCEND(quad *qd){
	
}

void    backpatch   (void){
    
}
void    generate_ADD            (quad *qd)	{ generate_instruction(add_v,qd); }
void    generate_SUB            (quad *qd)	{ generate_instruction(sub_v,qd); }
void    generate_MUL            (quad *qd)	{ generate_instruction(mul_v,qd); }
void    generate_DIV            (quad *qd)	{ generate_instruction(div_v,qd); }
void    generate_MOD            (quad *qd)	{ generate_instruction(mod_v,qd); }
void    generate_NEWTABLE       (quad *qd)	{ generate_instruction(newtable_v,qd); }
void    generate_TABLEGETELEM   (quad *qd)	{ generate_instruction(tablegetelem_v,qd); }
void    generate_TABLESETELEM   (quad *qd)	{ generate_instruction(tablesetelem_v,qd); }
void    generate_ASSIGN         (quad *qd)	{ generate_instruction(assign_v,qd); };
void    generate_NOP            (quad *qd)	{ instruction t = (*new instruction()); t.opcode = nop_v; emitInst(t);}
void    generate_JUMP           (quad *qd)	{ generate_relational(jump_v,qd); }
void    generate_IF_EQ          (quad *qd)	{ generate_relational(jeq_v,qd); }
void    generate_IF_NOTEQ       (quad *qd)	{ generate_relational(jne_v,qd); }
void    generate_IF_GREATER     (quad *qd)	{ generate_relational(jgt_v,qd); }
void    generate_IF_GREATEREQ   (quad *qd)	{ generate_relational(jge_v,qd); }
void    generate_IF_LESS        (quad *qd)	{ generate_relational(jlt_v,qd); }
void    generate_IF_LESSEQ      (quad *qd)	{ generate_relational(jle_v,qd); }


void    patch_incomplete_jump   ( void ){
    
}

//to dinei o savvidis
unsigned    consts_newstring    (std::string s){
    return (unsigned)atoi( s.c_str() );
}
//to dinei o savvidis
unsigned    consts_newnumber    (double n){
    return (unsigned)n;
}

 unsigned userfuncs_newfunc(Symbol * sym){
  
}

unsigned userfuncs_usedfunc(Symbol * f)
{
  
}


unsigned    libfuncs_newused    (std::string s){
    return (unsigned)atoi( s.c_str() );
}


//to dinei o savvidis
void        make_numberoperand  (vmarg* arg, double val){
   
}
//to dinei o savvidis
void        make_booloperand    (vmarg* arg, unsigned val){
   
}
//to dinei o savvidis
void        make_retvaloperand  (vmarg* arg){
    
}

string     vmopcodeToString        (vmopcode opcode){
    
}

std::string     vmargTypeToString       (vmarg_t tp){
    
}


void writeAll(void){
 const char* s="alpha.abc";
cout<<"malakes \n"; 
}

void        writeMagicNumber            (std::ofstream  *bin){
	 }

void  writeStringTable        ( ofstream *bin,ofstream *readableFile ){

}


void        writeNumberTable            (ofstream  *  bin,ofstream* readable){

	}
void        writeUserFuncTable          (ofstream   * bin,ofstream* readable){
	 

	}
void        writeLibFuncTable           (ofstream    *bin,ofstream* readable){

	}
void writeInstructionTable(ofstream *bin,ofstream* readable){
	
	}

void Read_MagicNumber(ifstream *  bin){
	}
void Read_Globals(ifstream *  bin){
	}
void Read_StringsTable(ifstream *  bin){
	
	}
void Read_NumbersTable(ifstream *  bin){
	}
void Read_UserfunctionsTable(ifstream *  bin){
}
void Read_LibraryfunctionsTable(ifstream *  bin){
	
}
void Read_InstructionsTable (ifstream *  bin){
	}

void avm_load(string filename){
	writeAll();  
}


//main(){
//writeAll();
//}

*/
