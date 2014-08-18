#ifndef BRAINFUCK_H
#define BRAINFUCK_H

#define DEFAULT_ARRAY_SIZE 30000
#define CELL char
#define DEFAULT_STACK_SIZE 50

#define ERR_HANDLE_STACK_THRESH 8

#define ERROR_INTERN 1
#define ERROR_SYNTAX 2

int bf_execute(char* program);
char* bf_read_file(char* filename);
#endif // BRAINFUCK_H
