/*
Assignment:
HW4 - Complete Parser and Code Generator for PL/0
(with Procedures, Call, and Else)
Author(s): Noah Rossetti, Whitney Evans
Language: C (only)
To Compile:
Scanner:
gcc -O2 -std=c11 -o lex lex.c
Parser/Code Generator:
gcc -O2 -std=c11 -o parsercodegen_complete parsercodegen_complete.c
Virtual Machine:
gcc -O2 -std=c11 -o vm vm.c
To Execute (on Eustis):
./lex <input_file.txt>
./parsercodegen_complete
./vm elf.txt
where:
<input_file.txt> is the path to the PL/0 source program
Notes:
- lex.c accepts ONE command-line argument (input PL/0 source file)
- parsercodegen_complete.c accepts NO command-line arguments
- Input filename is hard-coded in parsercodegen_complete.c
- Implements recursive-descent parser for extended PL/0 grammar
- Supports procedures, call statements, and if-then-else
- Generates PM/0 assembly code (see Appendix A for ISA)
- VM must support EVEN instruction (OPR 0 11)
- All development and testing performed on Eustis
Class: COP3402 - System Software - Fall 2025
Instructor: Dr. Jie Lin
Due Date: Friday, November 21, 2025 at 11:59 PM ET
*/

//defines symbol struct and makes size 500 array of them

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


typedef enum {
skipsym = 1 , // Skip / ignore token
identsym , // Identifier
numbersym , // Number
plussym , // +
minussym , // -
multsym , // *
slashsym , // /
eqsym , // =
neqsym , // <>
lessym , // <
leqsym , // <=
gtrsym , // >
geqsym , // >=
lparentsym , // (
rparentsym , // )
commasym , // ,
semicolonsym , // ;
periodsym , // .
becomessym , // :=
beginsym , // begin
endsym , // end
ifsym , // if
fisym , // fi
thensym , // then
whilesym , // while
dosym , // do
callsym , // call
constsym , // const
varsym , // var
procsym , // procedure
writesym , // write
readsym , // read
elsesym , // else
evensym // even
} TokenType ;


 FILE *OutputFile;

void program();
void Block();
void ConstDeclaration();
int VarDeclaration();
void ProcedureDeclaration();
void Statement();
void Condition();
void Expression();
void Term();
void Factor();


#define MAX_SYMBOL_TABLE_SIZE 500
#define CODE_SIZE 500
typedef struct{

int op; //Op code
int L; //Lexicographical level
int M; //modifier


} code;



typedef struct {
int kind; // const = 1, var = 2, proc = 3
char name[12]; // name up to 11 chars
int val; // number (ASCII value)
int level; // L level
int addr; // M address
int mark; // to indicate unavailable or deleted
} symbol;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

code text[CODE_SIZE];


char FileArray[500],ProccesedVarArray[100][12];

int tokenTracker = 0,varTracker = 0,symTracker = 0,proccesedTokenArray[500], cx = 0,level=0;




void emit(int op, int L, int M)
{
    if(cx > CODE_SIZE)
        printf("error in emit");
    //error(25);
    else
    {
        text[cx].op = op; //opcode
        text[cx].L = L; // lexicographical level
        text[cx].M = M; // modifier
        cx++;
}
}



//returns 1 if name is found -1 otherwise
int SymbolTableCheck(char* namesearch){

    int i = 0;
    int found = -1;

    //searches symbol table for the name
   do{

        if(strcmp(symbol_table[i].name,namesearch)==0)
        {


           found = 1;
           return i;
           }
        i++;
        if(i==499) return -1;
    } while(strcmp(symbol_table[i].name,namesearch)!=0);

    return i;
}

//Parsing functions start here

void program(){
    //could reserve spot for initial jump in code and just place it in once
    //procedures are sorted

Block();
    if(proccesedTokenArray[tokenTracker]!=periodsym)
    {
        fprintf(OutputFile, "Error: program must end with period");
        printf("Error: program must end with period");
        exit(-1);
    }
    else
    {
        for(int i = 0; i < 499; i++){
            //////////Double Check//////////////
            if(symbol_table[i].level==0){

            symbol_table[i].mark=1;

            }
        }
        emit(9,0,3);
    }


}


void Block(){

    printf(" Beginning of Block: %d",proccesedTokenArray[tokenTracker]);
    int jump_address= cx;
    emit(7,0,0);
ConstDeclaration(level);
int numvars = VarDeclaration(level);
ProcedureDeclaration(level);
printf("            cx: %d", cx);
text[jump_address].M = cx*3;

emit(6, 0, (3+numvars));
Statement();


}

void ConstDeclaration(){
    printf(" Beginning of Const: %d",proccesedTokenArray[tokenTracker]);
    char ident[12];


    //breaks down constants and places them in the symbol array
    if(proccesedTokenArray[tokenTracker]==constsym)
    {

        do{
            tokenTracker++;


            if(proccesedTokenArray[tokenTracker] ==identsym){


                    strcpy(ident, ProccesedVarArray[varTracker]);
                    varTracker++;
                    tokenTracker++;
                if(SymbolTableCheck(ident)!=-1)
                {
                    fprintf(OutputFile, "Error: symbol name has already been declared");
                    printf("Error: symbol name has already been declared");
                    exit(-1);
                }

                if(proccesedTokenArray[tokenTracker]==eqsym){

                    tokenTracker++;

                    if(proccesedTokenArray[tokenTracker] == numbersym)
                    {

                        tokenTracker++;
                        strcpy(symbol_table[symTracker].name, ident);

                        //have to change to ascii value later
                        symbol_table[symTracker].kind = 1;
                        symbol_table[symTracker].level = level;
                        symbol_table[symTracker].mark = 0;

                        symbol_table[symTracker].val = proccesedTokenArray[tokenTracker];

                        symTracker++;
                        tokenTracker++;
                    }
                    else{
                            fprintf(OutputFile, "Error: constants must be assigned an integer value");
                            printf("Error: constants must be assigned an integer value");
                            exit(-1);
                    }


                }
                else{
                    fprintf(OutputFile, "Error: constants must be assigned with =");
                    printf("Error: constants must be assigned with =");
                    exit(-1);

                }



            }
            else{

                fprintf(OutputFile, "Error: const, var, and read keywords must be followed by identifier");
                    printf("Error: const, var, and read keywords must be followed by identifier");
                    exit(-1);
                }



        }while(proccesedTokenArray[tokenTracker]==commasym);



        if(proccesedTokenArray[tokenTracker]!=semicolonsym)
        {
            fprintf(OutputFile, "Error: constant and variable declarations must be followed by a semicolon");
            printf("Error: constant and variable declarations must be followed by a semicolon");
            exit(-1);

        }
        tokenTracker++;
    }



}

int VarDeclaration(){
    int numvars = 0;
    printf(" Beginning of Var: %d",proccesedTokenArray[tokenTracker]);
    char ident[12];

    if(proccesedTokenArray[tokenTracker]==varsym)
    {



        do
        {
            tokenTracker++;

            numvars++;
            /////////////////new/////////////////////
            if(proccesedTokenArray[tokenTracker]!= identsym)
            {
                fprintf(OutputFile, "Error: const, var, and read keywords must be followed by identifier");
                printf("Error: const, var, and read keywords must be followed by identifier");
                exit(-1);
            }

            if(SymbolTableCheck(ProccesedVarArray[varTracker])!=-1)
            {
                fprintf(OutputFile, "Error: symbol name has already been declared");
                printf("Error: symbol name has already been declared");
                exit(-1);
            }
            strcpy(ident, ProccesedVarArray[varTracker]);
            tokenTracker++;
            varTracker++;

            strcpy(symbol_table[symTracker].name, ident);
            printf(" %s ",symbol_table[symTracker].name);
            //have to change to ascii value later
            symbol_table[symTracker].kind = 2;
            symbol_table[symTracker].level = level;
            symbol_table[symTracker].mark = 0;
            symbol_table[symTracker].addr = numvars+2;

            symbol_table[symTracker].val = 0;


            symTracker++;


        }while(proccesedTokenArray[tokenTracker]==commasym);
        //tokenTracker++;
        if(proccesedTokenArray[tokenTracker]!=semicolonsym)
        {
            fprintf(OutputFile, "Error: constant and variable declarations must be followed by a semicolon");
            printf("Error: constant and variable declarations must be followed by a semicolon");
            exit(-1);
        }

        tokenTracker++;


    }
    printf(" numvar: %d ", numvars);
return numvars;
}

void ProcedureDeclaration(){
    printf("**********Beginning of Procedure: %d %d %s***********",level, proccesedTokenArray[tokenTracker], ProccesedVarArray[varTracker]);
    char ident[12];
    int start_address = 0;

   // if(proccesedTokenArray[tokenTracker]==procsym){
    //    start_address = cx*3;
  //      emit(7,0,(cx+1)*3);
  //  }

    while(proccesedTokenArray[tokenTracker]==procsym){
            start_address = cx*3;
        //emit(7,0,(cx+1)*3);
        tokenTracker++;

        if (proccesedTokenArray[tokenTracker] != identsym){
            fprintf(OutputFile, "Error: call statement may only target procedures");
            printf("Error: call statement may only target procedures");
            exit(-1);


        }
        tokenTracker++;
        if (proccesedTokenArray[tokenTracker] != semicolonsym){

            fprintf(OutputFile, "Error: procedure declaration must be followed by a semicolon");
            printf("Error: procedure declaration must be followed by a semicolon");
            exit(-1);

        }
        tokenTracker++;

            printf("            before procedure in table %d                ",proccesedTokenArray[tokenTracker]);
            strcpy(ident, ProccesedVarArray[varTracker]);
            //tokenTracker++;
            varTracker++;

            strcpy(symbol_table[symTracker].name, ident);
            //have to change to ascii value later
            symbol_table[symTracker].kind = 3;
            symbol_table[symTracker].level = level;
            //probably wrong below
            symbol_table[symTracker].addr=(cx*3);


            symTracker++;
            level++;
            Block();
            level--;
            printf("            before procedure semicolon: %d                ",proccesedTokenArray[tokenTracker]);
            if (proccesedTokenArray[tokenTracker] != semicolonsym){

            fprintf(OutputFile, "Error: procedure declaration must be followed by a semicolon");
            printf("xxcxzError: procedure declaration must be followed by a semicolon");
            exit(-1);

            }
            tokenTracker++;

            for(int i = 0; i < 499; i++){
            //////////Double Check//////////////
            if(symbol_table[i].level==(level+1)){

            symbol_table[i].mark=1;

            }

        }


        emit(2,0,0);
        printf(" cx: %d", cx);
    }




}

void Statement(){
printf(" Beginning of Statement: %d",proccesedTokenArray[tokenTracker]);


    if(proccesedTokenArray[tokenTracker]==identsym)
    {

        int symidx = SymbolTableCheck(ProccesedVarArray[varTracker]);
        printf("        sym search: %s: ",ProccesedVarArray[varTracker]);

        if(symidx == -1)
        {
            fprintf(OutputFile, "Error: undeclared identifier");
            printf("Error: undeclared identifier");
            exit(-1);
        }
        if(symbol_table[symidx].kind!=2)
        {
             fprintf(OutputFile, "Error: only variable values may be altered");
            printf("Error: only variable values may be altered");
            exit(-1);

        }
        tokenTracker++;
        varTracker++;


        if(proccesedTokenArray[tokenTracker]!=becomessym)
        {
            fprintf(OutputFile, "Error: assignment statements must use :=");
            printf("Error: assignment statements must use :=");
            exit(-1);

        }
        tokenTracker++;
        Expression();

        emit(4,level-symbol_table[symidx].level,symbol_table[symidx].addr);

         printf("/////////////// after sto: %d //////////////////", proccesedTokenArray[tokenTracker]);
        return;

    }
    /////////////new///////////
     else if(proccesedTokenArray[tokenTracker]==callsym)
     {

         printf(" Beginning of call: %d",proccesedTokenArray[tokenTracker]);

        tokenTracker++;
        if(proccesedTokenArray[tokenTracker]!=identsym){

            //////change error///////////
            fprintf(OutputFile, "Error: assignment statements must use :=");
            printf("Error: assignment statements must use :=");
            exit(-1);

        }
        int idx = SymbolTableCheck(ProccesedVarArray[varTracker]);
        printf(" idx: %s", ProccesedVarArray[varTracker]);
        if(idx == -1){

             //////change error///////////
            fprintf(OutputFile, "Error: undeclared identifier");
            printf("Error: undeclared identifier");
            exit(-1);

        }
        if(symbol_table[idx].kind != 3){
                printf(" kind check: %d",symbol_table[idx].kind );
            printf(" Beginning of kind check: %d",proccesedTokenArray[tokenTracker]);
             //////change error///////////
            fprintf(OutputFile, "Error: call statement may only target procedures");
            printf("Error: call statement may only target procedures");
            exit(-1);

        }
        printf("                     idx::: %d  %d  %s                                   ",level,symbol_table[idx].level,symbol_table[idx].name);
        emit(5,level-symbol_table[idx].level,symbol_table[idx].addr);
        tokenTracker++;
        varTracker++;
        return;



     }

     else if(proccesedTokenArray[tokenTracker]==beginsym)
    {
        do{
            printf("begin loop start: %d ",proccesedTokenArray[tokenTracker]);
        tokenTracker++;
        Statement();
        printf("begin loop end: %d ",proccesedTokenArray[tokenTracker]);

        }while(proccesedTokenArray[tokenTracker]==semicolonsym);

        printf("before endsym: %d ",proccesedTokenArray[tokenTracker]);

        if(proccesedTokenArray[tokenTracker]!=endsym)
        {
            fprintf(OutputFile, "Error: begin must be followed by end");
            printf("Error: begin must be followed by end");
            exit(-1);

        }
        tokenTracker++;
        printf("after endsym: %d ",proccesedTokenArray[tokenTracker]);
        return;

    }
    else if(proccesedTokenArray[tokenTracker]==ifsym)
    {
         printf("ifsym opening: %d ", proccesedTokenArray[tokenTracker]);
        tokenTracker++;
        Condition();
        int jpcidx = cx;
        emit(8,0,0);
        if(proccesedTokenArray[tokenTracker]!=thensym)
        {
            fprintf(OutputFile, "Error: if must be followed by then");
            printf("Error: if must be followed by then");
            exit(-1);

        }
        tokenTracker++;
        printf(" +++++++++++++++++++++++token before this statement: %d ++++++++++", proccesedTokenArray[tokenTracker]);
        text[jpcidx].M = (cx*3);
        Statement();
        //look into + 3
        text[jpcidx].M = (cx*3)+3;
        //emit(7,0,(cx*3));
        printf("    //////// before elsym %d /////////", proccesedTokenArray[tokenTracker+1]);
        if(proccesedTokenArray[tokenTracker]!=elsesym)
        {

            fprintf(OutputFile, "Error: if statement must include else clause");
            printf("Error: if statement must include else clause");
            exit(-1);

        }
        tokenTracker++;
        Statement();
        //look into + 3
        emit(7,0,(cx*3)+3);

        if(proccesedTokenArray[tokenTracker]!=fisym)
        {
            fprintf(OutputFile, "else must be followed by fi");
            printf("else must be followed by fi");
            exit(-1);

        }

        tokenTracker++;
       //text[jpcidx].M = (cx*3);
        return;




    }
    else if(proccesedTokenArray[tokenTracker]==whilesym)
    {
        tokenTracker++;
        int loopidx = cx;
        Condition();
        if(proccesedTokenArray[tokenTracker] != dosym)
        {
        fprintf(OutputFile, "Error: while must be followed by do");
        printf("Error: while must be followed by do");
        exit(-1);

        }
        tokenTracker++;
        int jpcIdx = cx;
        emit(8, 0 ,0);
        Statement();
        emit(7,0,loopidx*3);
        text[jpcIdx].M= cx*3;

        return;
    }
    else if(proccesedTokenArray[tokenTracker]==readsym)
    {
        //////////////////////////////////////////////////////
        tokenTracker++;
        if(proccesedTokenArray[tokenTracker]!= identsym)
        {
            fprintf(OutputFile, "Error: only variable values may be altered");
            printf("Error: only variable values may be altered");
            exit(-1);

        }

        int symidx = SymbolTableCheck(ProccesedVarArray[varTracker]);

        if(symidx ==-1)
        {
            fprintf(OutputFile, "Error: undeclared identifier");
            printf("Error: undeclared identifier");
            exit(-1);
        }
        if(symbol_table[symidx].kind!=2)
        {
            fprintf(OutputFile, "Error: only variable values may be altered");
            printf("Error: only variable values may be altered");
            exit(-1);

        }
        varTracker++;
        tokenTracker++;
        emit(9,0,2);
        emit(4,level-symbol_table[symidx].level,symbol_table[symidx].addr);
        printf("/////////////// after sto: %d //////////////////", proccesedTokenArray[tokenTracker]);
        return;


    }
    else if(proccesedTokenArray[tokenTracker]==writesym)
    {
        tokenTracker++;
        Expression();
        emit(9,0,1);
        return;

    }


printf(" end of Statement: %d",proccesedTokenArray[tokenTracker]);

}

void Condition()
{
    printf(" Beginning of Condition: %d",proccesedTokenArray[tokenTracker]);
    //come back to this later

    //if(proccesedTokenArray[tokenTracker]==2) tokenTracker++;

    if(proccesedTokenArray[tokenTracker]==evensym)
    {
        tokenTracker++;
        Expression();
        emit(2,0,11);

    }
    else{
        Expression();

       if(proccesedTokenArray[tokenTracker]==eqsym)
        {
            tokenTracker++;
            Expression();
            emit(2,0,5);

        }
        else if(proccesedTokenArray[tokenTracker]==neqsym)
        {
            tokenTracker++;
            Expression();
            emit(2,0,6);

        }
        else if(proccesedTokenArray[tokenTracker]==lessym)
        {
            tokenTracker++;
            Expression();
            emit(2,0,7);

        }
        else if(proccesedTokenArray[tokenTracker]==leqsym)
        {
            tokenTracker++;
            Expression();
            emit(2,0,8);

        }
        else if(proccesedTokenArray[tokenTracker]==gtrsym)
        {
            tokenTracker++;
            Expression();
            emit(2,0,9);

        }
        else if(proccesedTokenArray[tokenTracker]==geqsym)
        {
            tokenTracker++;
            Expression();
            emit(2,0,10);

        }
        else{
            fprintf(OutputFile, "Error: condition must contain comparison operator");
            printf("Error: condition must contain comparison operator");

        }
    }

    printf(" end of Condition: %d",proccesedTokenArray[tokenTracker]);
}

//double check this
void Expression(){
    printf(" Beginning of Expression: %d",proccesedTokenArray[tokenTracker]);
    //if(proccesedTokenArray[tokenTracker]==2) tokenTracker++;

        Term();
    if(proccesedTokenArray[tokenTracker]==minussym)
    {
        tokenTracker++;
        Term();
        emit(2,0,2);
        while(proccesedTokenArray[tokenTracker]==plussym||proccesedTokenArray[tokenTracker]==minussym)
            {
                if(proccesedTokenArray[tokenTracker]==plussym)
                {
                    tokenTracker++;
                    Term();
                    emit(2,0,1);

                }
                else{
                    tokenTracker++;
                    Term();
                    emit(2,0,2);

                }

            }
    }
    else if(proccesedTokenArray[tokenTracker]==plussym)
    {
        tokenTracker++;
        Term();
       emit(2,0,1);
        while(proccesedTokenArray[tokenTracker]==plussym||proccesedTokenArray[tokenTracker]==minussym)
            {
                if(proccesedTokenArray[tokenTracker]==plussym)
                {
                    tokenTracker++;
                    Term();
                    emit(2,0,1);

                }
                else{
                    tokenTracker++;
                    Term();
                    emit(2,0,2);

                }

            }
    }

printf(" end of Expression: %d",proccesedTokenArray[tokenTracker]);
}

void Term()
{
    printf(" Beginning of Term: %d",proccesedTokenArray[tokenTracker]);
    Factor();
    while(proccesedTokenArray[tokenTracker]==multsym||proccesedTokenArray[tokenTracker]==slashsym)
    {

        if(proccesedTokenArray[tokenTracker]==multsym)
        {
            tokenTracker++;
            Factor();
            emit(2,0, 3);

        }
        else if(proccesedTokenArray[tokenTracker]==slashsym)
        {
            tokenTracker++;
            Factor();
            emit(2,0, 4);


        }
        //mod would go here
    }
     printf(" endof Term: %d",proccesedTokenArray[tokenTracker]);
}

void Factor(){
    printf(" Beginning of Factor: %d",proccesedTokenArray[tokenTracker]);

    if(proccesedTokenArray[tokenTracker]==identsym)
    {
        int symidx = SymbolTableCheck(ProccesedVarArray[varTracker]);

        if(symidx==-1)
        {
        fprintf(OutputFile, "Error: undeclared identifier");
        printf("Error: undeclared identifier");
        exit(-1);

        }

        if(symbol_table[symidx].kind==1)
        {
            emit(1,0,symbol_table[symidx].val);


        }
        else
        {
            emit(3,level-symbol_table[symidx].level,symbol_table[symidx].addr);


        }
        printf(" %s ", symbol_table[symidx].name);
        tokenTracker++;
        varTracker++;


    }
    else if(proccesedTokenArray[tokenTracker]==numbersym)
    {

        tokenTracker++;
        emit(1,0,proccesedTokenArray[tokenTracker]);
        tokenTracker++;

    }
    else if(proccesedTokenArray[tokenTracker]==lparentsym)
    {
        tokenTracker++;
        Expression();
        if(proccesedTokenArray[tokenTracker]!=rparentsym)
        {
            fprintf(OutputFile, "Error: right parenthesis must follow left parenthesis");
            printf("Error: right parenthesis must follow left parenthesis");
            exit(-1);

        }


        tokenTracker++;

    }
    else
    {
        fprintf(OutputFile, "Error: arithmetic equations must contain operands, parentheses, numbers, or symbols");
        printf("Error: arithmetic equations must contain operands, parentheses, numbers, or symbols");
        exit(-1);

    }

    printf(" end of Factor: %d",proccesedTokenArray[tokenTracker]);
}








int main(){

    FILE *input;

    input = fopen("tokens.txt", "r");

    OutputFile = fopen("elf.txt","w");

    char fileCharacter;
    int i = 0, j = 0 ;



    //This fills an array with the full input from the file
    while (( fileCharacter = fgetc(input)) != EOF){


        FileArray[i] = fileCharacter;

        i++;

    }

    //processes the char tokens into the int and var arrays
    for(j; j<i; j++)
    {
        int k = 0;

        //used to store variable names and tokens before sorting them between processedTokenArray and ProccesedVarArray
        char buffer[12] = {'\0'};

        //fills buffer array
        if(isdigit(FileArray[j]))
        {
            //printf("\n is digit ");
            while(FileArray[j]!='\0'&&FileArray[j]!='\n'&&isdigit(FileArray[j]))
            {

                buffer[k] = FileArray[j];
                j++;
                k++;

            }
            proccesedTokenArray[tokenTracker] = atoi(buffer);

            if(proccesedTokenArray[tokenTracker]==1)
            {
                if(proccesedTokenArray[tokenTracker-1]!=3)
                {
                printf("Error: Scanning error detected by lexer(skipsym present)");
                fprintf(OutputFile, "Error: Scanning error detected by lexer(skipsym present)");
                exit(-1);
                }

            }

            tokenTracker++;
        }
        else if(isalpha(FileArray[j]))
        {

            while(FileArray[j]!='\0'&&FileArray[j]!='\n'&&isalpha(FileArray[j]))
            {

                buffer[k] = FileArray[j];
                j++;
                k++;

            }
            strcpy(ProccesedVarArray[varTracker], buffer);



            varTracker++;


        }
        else
        {

        j++;

        }


    }



    tokenTracker = 0;
    varTracker = 0;
    program();
    printf("Assembly Code:");
    printf("\n\nLine\t  OP L M");
    fprintf(OutputFile, "%d %d %d", text[0].op, text[0].L, text[0].M);
    printf("\n%d\t JMP %d %d",0,  text[0].L, text[0].M);
    for(i=1; i<cx; i++){

        //printf("\n\n%d %d %d", text[i].op, text[i].L, text[i].M);

        fprintf(OutputFile, "\n%d %d %d", text[i].op, text[i].L, text[i].M);

        if(text[i].op == 1)
        {
            printf("\n%d\t LIT %d %d",i,  text[i].L, text[i].M);


        }
        else if(text[i].op == 2)
        {
            printf("\n%d\t OPR %d %d",i,  text[i].L, text[i].M);


        }
        else if(text[i].op == 3)
        {
            printf("\n%d\t LOD %d %d", i, text[i].L, text[i].M);


        }
        else if(text[i].op == 4)
        {
            printf("\n%d\t STO %d %d", i,  text[i].L, text[i].M);


        }
        else if(text[i].op == 5)
        {
            printf("\n%d\t CAL %d %d",i,  text[i].L, text[i].M);


        }
        else if(text[i].op == 6)
        {
            printf("\n%d\t INC %d %d", i,  text[i].L, text[i].M);


        }
        else if(text[i].op == 7)
        {
            printf("\n%d\t JMP %d %d",i,  text[i].L, text[i].M);


        }
        else if(text[i].op == 8)
        {
            printf("\n%d\t JPC %d %d",i,  text[i].L, text[i].M);


        }
        else if(text[i].op == 9)
        {
            printf("\n%d\t SYS %d %d", i, text[i].L, text[i].M);


        }


    }
    printf("\nSymbol Table:\n\n");
    printf("\nKind | Name\t| Value | Level | Address | Mark");
    printf("\n----------------------------------------------------");
    for(i=0; i < symTracker; i++)
    {

        printf("\n   %d|   \t%s|\t%d|\t%d|\t%d|\t%d",symbol_table[i].kind,symbol_table[i].name,symbol_table[i].val,symbol_table[i].level,symbol_table[i].addr,symbol_table[i].mark);

    }

    return 0;
}


