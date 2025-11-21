/*
Assignment :
vm . c - Implement a P - machine virtual machine

Authors : Noah Rossetti, Whitney Evans

Language : C ( only )

To Compile :
    gcc - O2 - Wall - std = c11 -o vm vm . c

To Execute ( on Eustis ) :
    ./ vm input . txt

where :
    input . txt is the name of the file containing PM /0 instructions ;
    each line has three integers ( OP L M )

Notes :
    - Implements the PM /0 virtual machine described in the homework
    instructions .
    - No dynamic memory allocation or pointer arithmetic .
    - Does not implement any VM instruction using a separate function .
    - Runs on Eustis .

Class : COP 3402 - Systems Software - Fall 2025

Instructor : Dr . Jie Lin

Due Date : Friday , September 12 th , 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#define PAS_SIZE 500
int pas[PAS_SIZE] = {0};

int base ( int BP , int L ) {
int arb = BP ; // activation record base
while ( L > 0) {
arb = pas [ arb ]; // follow static link
L--;
}
return arb ;
}

void printStack(int BP, int SP){

    //base case
    if(pas[BP-1]!=0){

     printStack(pas[BP], (BP+1));

    }
    printf(" | ");
    //prints activation record
    for(int i = BP; i>=SP; i--){

        printf(" %d ", pas[i]);

    }






}



int main(int argc, char *argv[]){

//if(argc!=2){

//    printf("error: incorrect number of arguments");
 //   return 0;

//}

//FILE *inputFile = fopen(argv[1], "r");
FILE *inputFile = fopen("elf.txt", "r");

int pc = 499, bp, sp, IR[3],  bufferReader = 0, instructionLoader = 499, Halt = 0;

char inputBuffer[4] = {0};

char inputCharacter;

//Reading in instructions

while (( inputCharacter = fgetc(inputFile)) != EOF){


    //separates instructions from space and newline characters

    if(isalnum(inputCharacter)){

        inputBuffer[bufferReader]=inputCharacter;
        bufferReader++;

    }
    else{

        if(bufferReader>0){

          pas[instructionLoader] = atoi(inputBuffer);
          instructionLoader--;

            for(int i = 0; i < 4; i++)
                inputBuffer[i] = '\0';
                bufferReader=0;
            }
    }


}

pas[instructionLoader] = atoi(inputBuffer);



fclose(inputFile);



//IR[0] is M, IR[1] is L and IR[2] is OP
//This will fetch and execute instructions

sp = instructionLoader;
bp =  sp - 1;
printf("\tL\t M    PC  BP  SP  Stack\n");
printf("Initial values:\t     %d  %d  %d\n", pc, bp, sp);

while(Halt != 1){

    char opCode[5];



    // loads instruction register
    IR[0]=pas[pc-2];
    IR[1]=pas[pc-1];
    IR[2]=pas[pc];

    pc = pc - 3;

    // literal push
    if(IR[2] == 1){

    sp = sp - 1;

    pas[sp] = IR[0];

    strcpy(opCode, "LIT");

    }

    // load
    else if(IR[2] == 3){

    sp = sp - 1;
    pas[sp] = pas[base(bp,IR[1])  - IR[0]];

    strcpy(opCode, "Lod");

    }

    // Store
    else if(IR[2] == 4){

    pas[base(bp, IR[1]) - IR[0]] = pas[sp];
    sp = sp + 1;

    strcpy(opCode, "STO");

    }

    //Call procedure
    else if(IR[2] == 5){

    pas[sp - 1] = base(bp, IR[1]);
    pas[sp - 2] = bp;
    pas[sp - 3] = pc;
    bp = sp - 1;
    pc = 499 - IR[0];

    strcpy(opCode, "CAL");

    //continue;

    }

    //Allocate locals onto stack
    else if(IR[2] == 6){



    sp = sp - IR[0];

     //strcpy(opCode, "JMP");
     strcpy(opCode, "INC");
    printf(" %s \t %d\t %d   %d  %d %d ", opCode, IR[1], IR[0], pc, bp, sp);
    printStack(bp, sp);
    printf(" \n");
    //strcpy(opCode, "INC");
    continue;
    }

    //Unconditional Jumo
    else if(IR[2] == 7){

    pc = 499 - IR[0];

    strcpy(opCode, "JMP");
    printf(" %s \t %d\t %d   %d  %d %d ", opCode, IR[1], IR[0], pc, bp, sp);
    printStack(bp, sp);
    printf(" \n");

    continue;

    }

    //Conditional Jump
    else if(IR[2] == 8){

        if(pas[sp] == 0){

            pc = 499 - IR[0];
            sp = sp + 1;

        }



           strcpy(opCode, "JPC");

    }



    else if(IR[2] == 9){

        //Output iteger

        if(IR[0] == 1){
            printf(" Output Result is: %d \n", pas[sp]);
            sp = sp + 1;


        }

        //Input integer
        else if(IR[0] == 2){

            int read;
            printf("Please enter an integer: ");
            scanf("%d", &read);
           sp = sp - 1;
           pas[sp] = read;



        }

        //ends program
        else if(IR[0]==3) Halt = 1;

        strcpy(opCode, "SYS");

    }

    // Arithmetic and Relational Operations (opcode 02, L = 0)
    else if(IR[2] == 2) {





        // RTN: return from subroutine (restoring caller's AR)
        if(IR[0] == 0) {
            sp = bp + 1;
            bp = pas[sp - 2];
            pc = pas[sp - 3];

            strcpy(opCode, "RTN");

        }
        // ADD: Addition
        else if(IR[0] == 1) {
            pas[sp + 1] = pas[sp + 1] + pas[sp];
            sp = sp + 1;

             strcpy(opCode, "ADD");

        }
        // SUB: Subtraction
        else if(IR[0] == 2) {
            pas[sp + 1] = pas[sp + 1] - pas[sp];
            sp = sp + 1;

             strcpy(opCode, "SUB");

        }
        // MUL: Multiplication
        else if(IR[0] == 3) {
            pas[sp + 1] = pas[sp + 1] * pas[sp];
            sp = sp + 1;

             strcpy(opCode, "MUL");

        }
        // DIV: Integer Division
        else if(IR[0] == 4) {
            pas[sp + 1] = pas[sp + 1] / pas[sp];
            sp = sp + 1;

             strcpy(opCode, "DIV");

        }
        // EQL: Equality Comparison (result 0/1)
        else if(IR[0] == 5) {
            pas[sp + 1] = (pas[sp + 1] == pas[sp]);
            sp = sp + 1;

             strcpy(opCode, "EQL");

        }
        // NEQ: Inequality Comparison (result 0/1)
        else if(IR[0] == 6) {
            pas[sp + 1] = (pas[sp + 1] != pas[sp]);
            sp = sp + 1;

             strcpy(opCode, "NEQ");

        }
        // LSS: Less-than Comparison (result 0/1)
        else if(IR[0] == 7) {
            pas[sp + 1] = (pas[sp + 1] < pas[sp]);
            sp = sp + 1;

             strcpy(opCode, "LSS");

        }
        // LEQ: Less-or-equal Comparison (result 0/1)
        else if(IR[0] == 8) {
            pas[sp + 1] = (pas[sp + 1] <= pas[sp]);
            sp = sp + 1;

             strcpy(opCode, "LEQ");

        }
        // GTR: Greater-than Comparison (result 0/1)
        else if(IR[0] == 9) {
            pas[sp + 1] = (pas[sp + 1] > pas[sp]);
            sp = sp + 1;

             strcpy(opCode, "GTR");

        }
        // GEQ: Greater-or-equal Comparison (result 0/1)
        else if(IR[0] == 10) {
            pas[sp + 1] = (pas[sp + 1] >= pas[sp]);
            sp = sp + 1;

             strcpy(opCode, "GEQ");

        }
        else if(IR[0] == 11) {

           if(pas[sp]%2 == 0)
            pas[(sp+1)] = 1;
           else
                pas[sp] = 0;
             strcpy(opCode, "EVEN");

        }


    }


    printf(" %s \t %d\t %d   %d  %d %d ", opCode, IR[1], IR[0], pc, bp, sp);
    printStack(bp, sp);
    printf(" \n");

}


    return(0);

}
