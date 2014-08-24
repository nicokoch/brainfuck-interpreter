#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "brainfuck.h"
#include "optimize.h"

/*
 * Pretty print the current brainfuck array
 */
void print_current_state(CELL * prog_array, CELL * ptr, int arr_size)
{
	int ptr_index = ptr - prog_array;
	int start_i =
	    ptr_index - ERR_HANDLE_STACK_THRESH <
	    0 ? 0 : ptr_index - ERR_HANDLE_STACK_THRESH;
	int end_i =
	    ptr_index + ERR_HANDLE_STACK_THRESH <
	    arr_size ? ptr_index + ERR_HANDLE_STACK_THRESH : arr_size - 1;
	int i;

	fprintf(stderr, "Indices:");
	for (i = start_i; i < end_i; i++) {
		fprintf(stderr, " %5d ", i);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "Array  :");
	for (i = start_i; i < end_i; i++) {
		fprintf(stderr, " %5d ", prog_array[i]);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "Pointer:");
	char ptr_symbol = '^';
	for (i = start_i; i < end_i; i++) {
		if (i == ptr_index) {
			fprintf(stderr, " %5c ", ptr_symbol);
		} else {
			fprintf(stderr, "       ");
		}
	}
	fprintf(stderr, "\n");
}

/*
 * Pretty print the code position of an error
 */
void print_code_position(char *prog_code, int code_pos)
{
	int code_len = strlen(prog_code);

	//get line number and line
	int line_cnt = 1;
	int line_start = 0;
	int i;
	for (i = 0; i < code_pos; i++) {
		if (prog_code[i] == '\n') {
			line_cnt++;
			line_start = i + 1;
		}
	}
	fprintf(stderr, "In line %d:\n", line_cnt);
	i = line_start;
	char cur = prog_code[i];
	while (cur != '\n' && i < code_len) {
		fprintf(stderr, "%c", cur);

		i++;
		cur = prog_code[i];
	}
	fprintf(stderr, "\n");
	int pos_in_line = code_pos - (line_start);
	i = 0;
	while (i < pos_in_line) {
		fprintf(stderr, " ");
		i++;
	}
	fprintf(stderr, "^\n");
}

/*
 * High level function to deal with brainfuck programming errors
 */
void deal_with_error(char *prog_code, int code_index, CELL * prog_array,
		     CELL * ptr)
{
	fprintf(stderr, "Current BRAINFUCK stack:\n");
	print_current_state(prog_array, ptr, DEFAULT_ARRAY_SIZE);
	print_code_position(prog_code, code_index);
}


#if USING_COMMAND_STRUCTURE
/*
 * Command types: "+" , "-" , ">" , "<" , "[" , "," , "." , "S" (S = start)
 * 
 * Example Command Structure
 * String: "++[->+<]>
 * Struct:
              '+' -> '+' -> '['                ->                  '>'
                            'S' -> '-' -> '>' -> '+' -> '<' -> ']'
 * In the diagram, a -> represents next_command and a command below another one represented child_command and parent_command
 */
struct Command{
    char type;
    struct Command *parent_command;
    struct Command *next_command;
    struct Command *child_command; //Used only if type=='['. Child command is guarenteed to have type of 'S'
};
struct Command *new_command(char type, struct Command *parent){
    struct Command *comm = malloc(sizeof(struct Command));
    comm->type = type;
    comm->parent_command = parent;
    comm->next_command = NULL;
    comm->child_command = NULL;
    return comm;
}

/*
 * This function converts a string of code into a stack data structure (Command)
 * Precondition: The code is minimized and balanced
 */
struct Command *build_command_struct(char *code){
    int code_len = strlen(code);
    
    struct Command *head = new_command('S', NULL); //Static, reamins as head
    struct Command *comm = head; //Moves around as the current command
    int i = 0;
    for (i = 0; i < code_len; i++) {
        struct Command *next = new_command(code[i], comm->parent_command);
        comm->next_command = next;
        comm = next;
        
        if(comm->type == '['){
            struct Command *child = new_command('S', comm);
            child->parent_command = comm;
            comm->child_command = child;
            comm = child;
        }
        else if(comm->type == ']'){
            comm = comm->parent_command;
        }
    }
    
    return head;
}

#else //USING_COMMAND_STRUCTURE

/*
 * O(n) function to get matching brackets -> TODO: replace this with a stack data structure
 */
int get_matching_close_index(char *string, int index)
{
	if (string[index] != '[') {
		fprintf(stderr,
			"Interpreter error: char at position %d does not equal '['\n",
			index);
		return -ERROR_INTERN;
	}
	unsigned int i = index;
	int opencnt = 0;
	while (i < strlen(string)) {
		char cur = string[i];
		if (cur == '[') {
			opencnt++;
		} else if (cur == ']') {
			opencnt--;
		}
		if (opencnt == 0) {
			return i;
		}
		i++;
	}
	fprintf(stderr,
		"Syntax error: no matching close bracket found for char at position %d\n",
		index);
	return -ERROR_SYNTAX;
}

int get_matching_open_index(char *string, int index)
{
	if (string[index] != ']') {
		fprintf(stderr,
			"Interpreter error: char at position %d does not equal ']'\n",
			index);
		return -ERROR_INTERN;
	}
	unsigned int i = index;
	int closecnt = 0;
	while (i != 0) {
		char cur = string[i];
		if (cur == ']') {
			closecnt++;
		} else if (cur == '[') {
			closecnt--;
		}
		if (closecnt == 0) {
			return i;
		}
		i--;
	}
	fprintf(stderr,
		"Syntax error: no matching open bracket found for char at position %d\n",
		index);
	return -ERROR_SYNTAX;
}

#endif //USING_COMMAND_STRUCTURE

/*
 * Read a file into heap string
 */
char *bf_read_file(char *filename)
{
	char *buffer = 0;
	long length;
	FILE *f = fopen(filename, "rb");

	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = malloc(length + 1);
		memset(buffer, '\0', length + 1);
		if (buffer) {
			int read_bytes = fread(buffer, 1, length, f);
			if (read_bytes != length) {
				fprintf(stderr,
					"Error while reading file: %s\n",
					filename);
				return NULL;
			}
		}
		fclose(f);
		return buffer;
	} else {
		fprintf(stderr, "File not found: %s\n", filename);
		return NULL;
	}
}

/*
 * Read the file, then remove non-brainfuck characters, effectively minimizing the file
 */
char *bf_read_file_and_minimize(char *filename)
{
	char *file_contents = bf_read_file(filename);
	if (!file_contents)
		return NULL;
	file_contents = bf_minimize_file(file_contents);
	return file_contents;
}

/*
 * Create an empty program array
 */
CELL *bf_create_array(int arr_size)
{
	CELL *result = malloc(arr_size * sizeof(CELL));
	memset(result, '\0', arr_size);	//suck it, calloc
	return result;
}

void bf_destroy_array(CELL * prog_array)
{
	free(prog_array);
}

/*
 * Execute brainfuck code: this is still very inefficient.
 * TODO: Optimizations, check array bounds?, dynamic array size?
 * 
 * Method signature changed on 19.08.2014 to enable interactive mode support
 * 
 * @args:	program: the bf code to execute
 * 			prog_array: the current program array (get it from bf_create_array)
 * 			ptr: the current instruction pointer (in full execution mode declare it like CELL* ptr = prog_array)
 * 
 * @returns: The resulting instruction pointer
 */


CELL *bf_execute(char *program, CELL * prog_array, CELL * ptr)
{
#if USING_COMMAND_STRUCTURE
    
    struct Command *command_struct = build_command_struct(program);

    while (command_struct->next_command) {
        command_struct = command_struct->next_command;
        switch (command_struct->type) {
            case '+':
                ++*ptr;
                break;
            case '-':
                --*ptr;
                break;
            case '>':
                ptr++;
                break;
            case '<':
                ptr--;
                break;
            case '.':
                putchar(*ptr);
                break;
            case ',':
                *ptr = getchar();
                break;
            case '[':
                if(*ptr != 0){
                    command_struct = command_struct->child_command;
                }
                break;
            case ']':
                if(*ptr != 0){
                    command_struct = command_struct->parent_command->child_command; //Return to the 'S'
                }
                else{
                    command_struct = command_struct->parent_command; 
                }
            default:
                break;
        }
    }
    
#else //USING_COMMAND_STRUCTURE
    
	int i = 0;
	int prev_i;
	while (i < strlen(program)) {
		prev_i = i;
		char token = program[i];
		switch (token) {
		case '<':
			ptr--;
			break;
		case '>':
			ptr++;
			break;
		case '+':
			++*ptr;
			break;
		case '-':
			--*ptr;
			break;
		case '.':
			putchar(*ptr);
			break;
		case ',':
			*ptr = getchar();
			break;
		case '[':
			if (*ptr == 0) {
				i = get_matching_close_index(program, i);
			}
			break;
		case ']':
			if (*ptr != 0) {
				i = get_matching_open_index(program, i);
			}
			break;
		}
		if (i < 0) {
			deal_with_error(program, prev_i, prog_array, ptr);
			return NULL;
		}
		i++;
	}
    
#endif //USING_COMMAND_STRUCTURE
	return ptr;
}
