#ifndef BRAINFUCK_H
#define BRAINFUCK_H

#define DEFAULT_ARRAY_SIZE 30000
#define CELL char
#define DEFAULT_STACK_SIZE 50

#define ERR_HANDLE_STACK_THRESH 8

#define ERROR_INTERN 1
#define ERROR_SYNTAX 2

CELL* bf_execute(char* program, CELL* prog_array, CELL* ptr);
char* bf_read_file(char* filename);
char* bf_read_file_and_minimize(char* filename);
char* bf_create_array(int arr_size);
void bf_destroy_array(char* array);
void print_current_state(CELL *prog_array, CELL* ptr, int arr_size);
#endif // BRAINFUCK_H
