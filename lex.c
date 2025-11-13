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


char initialFileArray[500], processedFileArray[500], identNumArray[50][50], errorArray[50][25];
int tokenArray[100], identNumRow = 0, identNumColumn = 0, errorArrayTracker = 0;



int main(int argc, char *argv[]){

int filePlaceHolder = 0;

FILE *inputFile = fopen(argv[1], "r");
//FILE *inputFile = fopen("input", "r");
    char fileCharacter;
    int i = 0, j = 0, arraySizetracker;

      //printf("Source Program:\n\n");





    //This fills an array with the full input from the file
    while (( fileCharacter = fgetc(inputFile)) != EOF){

        initialFileArray[i] = fileCharacter;
        //printf("%c", initialFileArray[i]);
        i++;

    }



    //Fills array with the input while excluding ignored elements
    for(i = 0; i < 500; i++){


        //checks for and skips excluded elements
        if(initialFileArray[i] == ' ' || initialFileArray[i] == '\n' ||initialFileArray[i] == '\t' || initialFileArray[i] == '\r' || initialFileArray[i] == '\0')
        {
            //places tilde in place of ignored elements to keep track of spacing between identifiers, numbers, and words
           processedFileArray[j] = '~';


            j++;


        }
        //checks if start of comment
        else if(initialFileArray[i] == '/'){

            if(initialFileArray[i+1] == '*'){

                //skips forward past / and *
                filePlaceHolder = i;
                i = i + 2;

                //skips content of comment
                while(initialFileArray[i] != '*' ||initialFileArray[i+1] != '/'){
                i++;
                //i++;

                if(i==500){
                    i =filePlaceHolder;
                    processedFileArray[j] = '/';
                    j++;
                    processedFileArray[j] = '*';
                    //new /////////////////////////////////////////////////////
                    j++;


                    i++;
                    break;
                }
                }
                i++;


            }

            else{
                processedFileArray[j] = initialFileArray[i];



                j++;

            }

        }


        else{

            processedFileArray[j] = initialFileArray[i];



            j++;

        }

    }

    // Keeps track of length of array and resets j
    arraySizetracker = j;
    j = 0;

    /*for(i=0; i<arraySizetracker; i++){

        printf(" %c ", processedFileArray[i]);

    }*/



    ////////////////////////////////////////////////////////////////////////







    // Start of the automata
    for(i = 0; i < arraySizetracker; i++){

        // First decision of the automata is between whether the character is  number/letter or a symbol

        // This will handler identifiers, numbers and reserved words
        if(processedFileArray[i] == '~') continue;
        else if(isalnum(processedFileArray[i])){


            if(isdigit(processedFileArray[i])){
                char *numBuffer = (char *)malloc(sizeof(char)*50);
                int bufferIterator = 0;

                numBuffer[bufferIterator] = processedFileArray[i];
                bufferIterator++;
                while(isdigit(processedFileArray[i+1])){
                    numBuffer[bufferIterator] = processedFileArray[i+1];

                    identNumColumn++;

                    bufferIterator++;
                    i++;

                }
                //adds terminator to the string
                numBuffer[bufferIterator] ='\0';
               // printf(" %s ", numBuffer);


                //error detector for number being too long
                if(identNumColumn > 4){

                    tokenArray[j]=1;
                    strcpy(errorArray[errorArrayTracker], "Number too Long\0");
                    strcpy(identNumArray[identNumRow], numBuffer);
                    errorArrayTracker++;

                }
                else{

                strcpy(identNumArray[identNumRow], numBuffer);
                tokenArray[j]=3;

                }

                free(numBuffer);
                identNumRow++;
                j++;
                identNumColumn=0;
            }



    // Tokenizes identifiers and reserved words
            else if(isalpha(processedFileArray[i])){
               int length = 0;
               char lexeme[50] = {'\0'}; // Temporary buffer
               //char lexeme[50];
               int continueflag = 0;

            //checks first if it is a reserved word
            while (isalnum(processedFileArray[i])) {

                    if (length < 49) {
                    lexeme[length++] = processedFileArray[i];
                    }
                    i++;



               // Checking reserved words
               if (strncmp(lexeme, "begin", 5) == 0) {
                    tokenArray[j++] = beginsym;
                    continueflag = 1;
                    break;

               }
               else if (strncmp(lexeme, "end", 3) == 0) {
                    tokenArray[j++] = endsym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "if", 2) == 0) {
                    tokenArray[j++] = ifsym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "fi", 2) == 0) {
                    tokenArray[j++] = fisym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "then", 4) == 0) {
                    tokenArray[j++] = thensym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "while",5) == 0) {
                    tokenArray[j++] = whilesym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "do", 2) == 0) {
                    tokenArray[j++] = dosym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "call", 4) == 0) {
                    tokenArray[j++] = callsym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "const", 5) == 0) {
                    tokenArray[j++] = constsym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "var", 3) == 0) {
                    tokenArray[j++] = varsym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "procedure", 9) == 0) {
                    tokenArray[j++] = procsym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "write", 5) == 0) {
                    tokenArray[j++] = writesym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "read", 4) == 0) {
                    tokenArray[j++] = readsym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "else", 4) == 0) {
                    tokenArray[j++] = elsesym;
                    continueflag = 1;
                    break;
               }
               else if (strncmp(lexeme, "even", 4) == 0) {
                    tokenArray[j++] = evensym;
                    continueflag = 1;
                    break;
               }
            }

                lexeme[length] = '\0';

                 i--;

                    // Otherwise treated as identifier
                if(continueflag == 1) continue;

                else if (strlen(lexeme) > 11) {
                    tokenArray[j] = 1;
                    strcpy(identNumArray[identNumRow], lexeme);
                    strcpy(errorArray[errorArrayTracker], "Too many characters");
                    errorArrayTracker++;
                    identNumRow++;
                    j++;
                } else {
                    tokenArray[j++] = identsym;
                    strcpy(identNumArray[identNumRow++], lexeme);
                }

            }


        }

        // This will handle all symbols
        else{


            if(processedFileArray[i] == '+'){

                tokenArray[j] = plussym;
                j++;

            }
            else if(processedFileArray[i] == '-'){

                tokenArray[j] = minussym;
                j++;

            }
            else if(processedFileArray[i] == '*'){

                tokenArray[j] = multsym;
                j++;


            }
            else if(processedFileArray[i] == '/'){

                tokenArray[j] = slashsym;
                j++;

            }
            else if(processedFileArray[i] == '='){

                tokenArray[j] = eqsym;
                j++;

            }
            else if(processedFileArray[i] == '<'){
                    //going to have a few options

                    if(processedFileArray[i+1] == '>'){

                        tokenArray[j] = neqsym;
                        j++;
                        i++;

                    }

                    else if(processedFileArray[i+1] == '='){

                        tokenArray[j] = leqsym;
                        j++;
                        i++;
                    }

                    else {

                        tokenArray[j] = lessym;
                        j++;

                    }



            }
            else if(processedFileArray[i] == '>'){
                //going to have a few options
                 if(processedFileArray[i+1] == '='){

                    tokenArray[j] = geqsym;
                    j++;
                    i++;

                }
                else{
                    //new://////////////////////////////////////////////////////////////////////
                    tokenArray[j] = gtrsym;
                    j++;

                }


            }
            else if(processedFileArray[i] == '('){

                tokenArray[j] = lparentsym;
                j++;

            }
            else if(processedFileArray[i] == ')'){

                tokenArray[j] = rparentsym;
                j++;

            }
            else if(processedFileArray[i] == ','){

                tokenArray[j] = commasym;
                j++;

            }
            else if(processedFileArray[i] == ';'){

                tokenArray[j] = semicolonsym;
                j++;

            }
            else if(processedFileArray[i] == '.'){

                tokenArray[j] = periodsym;
                j++;

            }
            else if(processedFileArray[i] == ':'){

                tokenArray[j] = becomessym;
                j++;
                i++;

            }
            else{
                char buffer[2];
                buffer[0] = processedFileArray[i];
                buffer[1] = '\0';
                tokenArray[j] = 1;
                strcpy(errorArray[errorArrayTracker], "Invalid Symbol\0");
                strcpy(identNumArray[identNumRow], buffer);
                errorArrayTracker++;
                j++;
                identNumRow++;

            }



        }




    }

     identNumRow = 0;
     errorArrayTracker = 0;


// This section prints the output
    int tokenNum = j;
    //printf("\n\nLexeme Table\nLexeme\t  token type");
    /*
     for(i = 0; i<tokenNum; i++){



        switch(tokenArray[i])
        {
            // 1 2 and 3 will search the identnum array
            case 1:
           //printf("\n%s\t\t%s", identNumArray[identNumRow], errorArray[errorArrayTracker]);
            identNumRow++;
            errorArrayTracker++;
            break;

            case 2:
           //printf("\n%s\t\t 2", identNumArray[identNumRow]);
            identNumRow++;
            break;

            case 3:
           // printf("\n%s\t\t 3", identNumArray[identNumRow]);
            identNumRow++;
            break;

            case 4:
           // printf("\n+\t\t 4");
            break;

            case 5:
           // printf("\n-\t\t 5");
            break;

            case 6:
           // printf("\n*\t\t 6");
            break;

            case 7:
            //printf("\n/\t\t 7");
            break;

            case 8:
            //printf("\n=\t\t 8");
            break;
            case 9:
            //printf("\n<>\t\t 9");
            break;

            case 10:
            //printf("\n<\t\t 10");
            break;

            case 11:
            //printf("\n<=\t\t 11");
            break;

            case 12:
           // printf("\n>\t\t 12");
            break;

            case 13:
          //  printf("\n>=\t\t 13");
            break;

            case 14:
           // printf("\n(\t\t 14");
            break;

            case 15:
            printf("\n)\t\t 15");
            break;

            case 16:
            printf("\n,\t\t 16");
            break;

            case 17:
            printf("\n;\t\t 17");
            break;

            case 18:
            printf("\n.\t\t 18");
            break;

            case 19:
            printf("\n:=\t\t 19");
            break;

            case 20:
            printf("\nbegin\t\t 20");
            break;

            case 21:
            printf("\nend\t\t 21");
            break;

            case 22:
            printf("\nif\t\t 22");
            break;

            case 23:
            printf("\nfi\t\t 23");
            break;

            case 24:
            printf("\nthen\t\t 24");
            break;

            case 25:
            printf("\nwhile\t\t 25");
            break;

            case 26:
            printf("\ndo\t\t 26");
            break;

            case 27:
            printf("\ncall\t\t 27");
            break;

            case 28:
            printf("\nconst\t\t 28");
            break;

            case 29:
            printf("\nvar\t\t 29");
            break;

            case 30:
            printf("\nprocedure\t 30");
            break;

            case 31:
            printf("\nwrite\t\t 31");
            break;

            case 32:
            printf("\nread\t\t 32");
            break;

            case 33:
            printf("\nelse\t\t 33");
            break;

             case 34:
            printf("\neven\t\t 34");
            break;






        }

    }*/
    FILE *outputFile = fopen("tokens.txt", "w");


     identNumRow = 0;
    //printf("\n\nToken List\n\n");

      for(i = 0; i<tokenNum; i++){
        if(tokenArray[i] == 1){

            fprintf(outputFile, "%d\n", tokenArray[i]);
           // printf(" %d ", tokenArray[i]);
            identNumRow++;

        }
        else if(tokenArray[i] == 3){
            fprintf(outputFile, "3 %s\n", identNumArray[identNumRow]);
           // printf(" 3  %s", identNumArray[identNumRow]);
            identNumRow++;


        }
        else if(tokenArray[i] == 2){
            fprintf(outputFile, "2 %s\n", identNumArray[identNumRow]);
           // printf(" 2  %s ", identNumArray[identNumRow]);
            identNumRow++;

        }
        else{

           fprintf(outputFile, "%d\n", tokenArray[i]);
           // printf(" %d ", tokenArray[i]);
        }

    }
    printf("\n");


}
