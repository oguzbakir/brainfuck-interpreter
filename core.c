#include <stdio.h>
#include <stdlib.h>

//Compile and execute signal defns
#define COMPILATION_SUCCESS         0
#define COMPILATION_FAILURE         1
#define EXECUTE_SUCCESS             2 //TODO: Handle execute return codes
#define EXECUTE_FAILURE             3

//Size defns
#define STACK_SIZE                  4096

//Stack defns
#define STACK_POP()                 (STACK[--SP])
#define STACK_PUSH()                (STACK[SP++])
#define STACK_IS_EMPTY()            (SP == 0)
#define STACK_IS_FULL()             (SP == STACK_SIZE)


static unsigned long STACK[STACK_SIZE];
static unsigned int sp = 0;

int compile_brainfuck(FILE* fp) {
    return COMPILATION_SUCCESS;
}

int execute_brainfuck() {
    return EXECUTE_SUCCESS;
}

int main(int argc, const char * argv[])
{
    int status;
    FILE *fp;
    fp = fopen(argv[1], "r");
    
    if (argc != 2) {
        printf("Usage: ./a.out (filename)\n");
        exit(EXIT_FAILURE);
    }
    
    if (fp == NULL){
        printf("File %s not found\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    
    status = compile_brainfuck(fp);
    fclose(fp);
    
    if (status == COMPILATION_SUCCESS) status = execute_brainfuck();
    if (status == COMPILATION_FAILURE){
        printf("Compilation Error!\n");  //TODO: Somehow handle errors.
        exit(EXIT_FAILURE);
    }
    return status;
}
