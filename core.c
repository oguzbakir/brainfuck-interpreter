#include <stdio.h>
#include <stdlib.h>

//Compile and execute signal defns
#define COMPILATION_SUCCESS         0
#define COMPILATION_FAILURE         1
#define EXECUTE_SUCCESS             2 //TODO: Handle execute return codes
#define EXECUTE_FAILURE             3

//Size defns
#define STACK_SIZE                  4096
#define PROGRAM_SIZE                65535

//Stack defns
#define STACK_POP()                 (STACK[--SP])
#define STACK_PUSH(A)                (STACK[SP++] = A)
#define STACK_IS_EMPTY()            (SP == 0)
#define STACK_IS_FULL()             (SP == STACK_SIZE)

//Operations
#define INCREMENT_PTR               1
#define DECREMENT_PTR               2
#define INCREMENT_BYTE_PTR          3
#define DECREMENT_BYTE_PTR          4
#define OUTPUT_PTR                  5
#define INPUT_PTR                   6
#define JUMP_FORWARD_PTR            7
#define JUMP_BACKWARD_PTR           8
#define END                         9


struct instr_t
{
    unsigned int operator;
    unsigned int operand;
};

static struct instr_t program[PROGRAM_SIZE];
static unsigned int STACK[STACK_SIZE];
static unsigned int sp = 0;

int compile_brainfuck(FILE* fp) {
    unsigned int pc = 0, jmp;
    char c = getc(fp);
    while (c != EOF && pc < PROGRAM_SIZE){
        switch (){
            case '>': program[pc].operator = INCREMENT_PTR; break;
            case '<': program[pc].operator = DECREMENT_PTR; break;
            case '+': program[pc].operator = INCREMENT_BYTE_PTR; break;
            case '-': program[pc].operator = DECREMENT_BYTE_PTR; break;
            case '.': program[pc].operator = OUTPUT_PTR; break;
            case ',': program[pc].operator = INPUT_PTR; break;
            case '[': program[pc].operator = JUMP_FORWARD_PTR;
                if (STACK_IS_FULL()) return COMPILATION_FAILURE; //TODO: Edit signal
                STACK_PUSH();
                break;
            case ']':
                if (STACK_IS_EMPTY) return COMPILATION_FAILURE; //TODO: Edit signal
                jmp = STACK_POP();
                program[pc].operator = JUMP_BACKWARD_PTR;
                program[pc].operand = jmp;
                program[jmp].operand = pc;
                break;
            default: pc--; break;
        }
        pc++; //Increment program counter every cycle
    }
    if (pc == PROGRAM_SIZE) return COMPILATION_FAILURE; //TODO: Handle signals
    if (!STACK_IS_EMPTY) return COMPILATION_FAILURE; //TODO: Handle signals

    program[pc].operator = END;

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
