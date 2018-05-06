#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//Compile and execute signal defns
#define COMPILATION_SUCCESS         0
#define COMPILATION_FAILURE         1
#define COMPILATION_OVERFLOW        2
#define COMPILATION_STACKOVERFLOW   3
#define COMPILATION_WTF             9
#define EXECUTE_SUCCESS             10
#define EXECUTE_FAILURE             11
#define EXECUTE_OVERFLOW            12
#define EXECUTE_WTF                 19
#define J_OK                        20
#define J_ERR                       21

//Size defns
#define STACK_SIZE                  4096
#define PROGRAM_SIZE                65535
#define BINARY_SIZE                 65535

//Stack defns
#define STACK_POP()                 (STACK[--SP])
#define STACK_PUSH(A)               (STACK[SP++] = A)
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
static unsigned int STACK[STACK_SIZE], SP = 0;
static unsigned int errLine;

int check_jump(FILE* fp){
    int c;
    int ret = 0;
    int cc = 0;
    while((c = getc(fp)) != EOF){
        if (ret < 0) break;
        if (c == '['){
            ret++; cc++;
            errLine = cc;
            continue;
        }
        if (c == ']'){
            ret--; cc++;
            errLine = cc;
            continue;
        }
        cc++;
    }
    if (ret != 0) return J_ERR;
    return J_OK;
}

int compile_brainfuck(FILE* fp) {
    unsigned int pc = 0, jmp;
    int c;
    while ((c = getc(fp)) != EOF && pc < PROGRAM_SIZE){
        switch (c){
            case '>': program[pc].operator = INCREMENT_PTR; break;
            case '<': program[pc].operator = DECREMENT_PTR; break;
            case '+': program[pc].operator = INCREMENT_BYTE_PTR; break;
            case '-': program[pc].operator = DECREMENT_BYTE_PTR; break;
            case '.': program[pc].operator = OUTPUT_PTR; break;
            case ',': program[pc].operator = INPUT_PTR; break;
            case '[': program[pc].operator = JUMP_FORWARD_PTR;
                if (STACK_IS_FULL()) return COMPILATION_STACKOVERFLOW;
                STACK_PUSH(pc);
                break;
            case ']':
                if (STACK_IS_EMPTY()) return COMPILATION_WTF;
                jmp = STACK_POP();
                program[pc].operator = JUMP_BACKWARD_PTR;
                program[pc].operand = jmp;
                program[jmp].operand = pc;
                break;
            default: pc--; break;
        }
        pc++; //Increment program counter every cycle
    }
    if (pc == PROGRAM_SIZE) return COMPILATION_OVERFLOW;
    if (!STACK_IS_EMPTY()) return COMPILATION_WTF;

    program[pc].operator = END;

    return COMPILATION_SUCCESS;
}

int execute_brainfuck() {
    unsigned short bin[BINARY_SIZE], pc = 0, ptr = BINARY_SIZE;
    while (--ptr) bin[ptr] = 0; //Fill binary with zeros
    while (program[pc].operator != END && ptr < BINARY_SIZE){
        switch (program[pc].operator){
            case INCREMENT_PTR: ptr++; break;
            case DECREMENT_PTR: ptr--; break;
            case INCREMENT_BYTE_PTR: bin[ptr]++; break;
            case DECREMENT_BYTE_PTR: bin[ptr]--; break;
            case OUTPUT_PTR: putchar(bin[ptr]); break;
            case INPUT_PTR: bin[ptr] = (unsigned int) getchar(); break;
            case JUMP_FORWARD_PTR: 
                if (!bin[ptr]) pc = program[pc].operand;
                break;
            case JUMP_BACKWARD_PTR:
                if (bin[ptr]) pc = program[pc].operand;
                break;
            default: {
                return EXECUTE_WTF;
                break;
            }
        }
        pc++; //Increment program counter every cycle
    }
    if (ptr == BINARY_SIZE) return EXECUTE_OVERFLOW;

    return EXECUTE_SUCCESS;
}

int main(int argc, const char * argv[])
{
    int status, check;
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

    check = check_jump(fp);
    if (check == J_ERR){
        printf("Character at %u is not closed\n", errLine);
        exit(EXIT_FAILURE);
    }

    rewind(fp);
    
    status = compile_brainfuck(fp);
    fclose(fp);

    switch (status){
        case COMPILATION_SUCCESS: {
            status = execute_brainfuck();
            break;
        }
        case COMPILATION_OVERFLOW: {
            printf("Compilation overflow. Max size is 65Kb\n");
            exit(EXIT_FAILURE);
            break;
        }
        case COMPILATION_STACKOVERFLOW: {
            printf("Stack overflown, please check your code\n");
            exit(EXIT_FAILURE);
            break;
        }
        case COMPILATION_FAILURE: {
            printf("Compilation Error!\n");  //TODO: Somehow handle errors.
            exit(EXIT_FAILURE);
            break;
        }
        case COMPILATION_WTF: {
            printf("WTF?\n");
            raise(SIGSEGV);
            break;
        }
        default: {
            printf("WTF?\n");
            raise(SIGSEGV);
            break;
        }
    }

    switch (status){
        case EXECUTE_SUCCESS: break;
        case EXECUTE_FAILURE: {
            printf("Execution failed\n");
            exit(EXIT_FAILURE);
            break;
        }
        case EXECUTE_OVERFLOW:{
            printf("Binary overflow. Max size is 65Kb\n");
            exit(EXIT_FAILURE);
            break;
        }
        case EXECUTE_WTF:{
            printf("WTF?\n");
            raise(SIGSEGV);
            break;
        }
        default: {
            printf("WTF?\n");
            raise(SIGSEGV);
            break;
        }
    }
    
    return status;
}
