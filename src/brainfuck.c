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
void
deal_with_error(char *prog_code, int code_index, CELL * prog_array, CELL * ptr)
{
	fprintf(stderr, "Current BRAINFUCK stack:\n");
	print_current_state(prog_array, ptr, DEFAULT_ARRAY_SIZE);
	print_code_position(prog_code, code_index);
}

/*
 * Command types: "+" , "-" , ">" , "<" , "[" , "," , "." , "S" (S = start)
 * 
 * Example Command Structure
 * String: "++[->+<]>
 * Struct:
       'S' -> '+',2 -> '['                ->                  '>'
                       'S' -> '-' -> '>' -> '+' -> '<' -> ']'
 * In the diagram, a -> represents next_command and a command below another one represented child_command and parent_command, and the '+',2 means a '+' command with a magnitude of 2
 */
struct Command {
	char type;
	int magnitude;
	struct Command *parent_command;
	struct Command *next_command;
	struct Command *child_command;	//Used only if type=='['. Child command is guarenteed to have type of 'S'
};
struct Command *new_command(char type, struct Command *parent)
{
	struct Command *comm = malloc(sizeof(struct Command));
	comm->type = type;
	comm->magnitude = 1;
	comm->parent_command = parent;
	comm->next_command = NULL;
	comm->child_command = NULL;
	return comm;
}

/*
 * This function converts a string of code into a stack data structure (Command)
 * Precondition: The code is minimized and balanced
 */
struct Command *build_command_struct(char *code)
{
	int code_len = strlen(code);

	struct Command *head = new_command('S', NULL);	//Static, reamins as head
	struct Command *comm = head;	//Moves around as the current command
	int i = 0;
	for (i = 0; i < code_len; i++) {
		// Quick optimization for the commands '+','-','>','<'
		// If they are repeated over and over again then instead of making a new command, just increase the magnitude of the previous command.
		if ((i != 0) && (code[i] == code[i - 1]) &&
		    (code[i] == '+' || code[i] == '-' || code[i] == '>'
		     || code[i] == '<')) {
			comm->magnitude++;
			continue;
		}
		// If no optimization then create a new command struct with the same parent as the previous
		struct Command *next =
		    new_command(code[i], comm->parent_command);
		comm->next_command = next;
		comm = next;

		if (comm->type == '[') {
			struct Command *child = new_command('S', comm);
			child->parent_command = comm;
			comm->child_command = child;
			comm = child;
		} else if (comm->type == ']') {
			comm = comm->parent_command;
		}
	}

	return head;
}

/*
 * Free the memory of a command struct linked list created by build_command_struct()
 */
void destroy_command_struct(struct Command *head)
{
	if (head->child_command) {
		destroy_command_struct(head->child_command);
	}
	if (head->next_command) {
		destroy_command_struct(head->next_command);
	}
	free(head);
}

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
 * Execute brainfuck code
 * TODO: Optimizations, check array bounds?, dynamic array size?, add error handling
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

	struct Command *command_struct = build_command_struct(program);
	struct Command *head = command_struct;
	while (command_struct->next_command) {
		command_struct = command_struct->next_command;
		switch (command_struct->type) {
		case '+':
			*ptr += command_struct->magnitude;
			break;
		case '-':
			*ptr -= command_struct->magnitude;
			break;
		case '>':
			ptr += command_struct->magnitude;
			break;
		case '<':
			ptr -= command_struct->magnitude;
			break;
		case '.':
			putchar(*ptr);
			break;
		case ',':
			*ptr = getchar();
			break;
		case '[':
			if (*ptr != 0) {
				command_struct = command_struct->child_command;
			}
			break;
		case ']':
			if (*ptr != 0) {
				command_struct = command_struct->parent_command->child_command;	//Return to the 'S'
			} else {
				command_struct = command_struct->parent_command;
			}
		default:
			break;
		}
	}
	destroy_command_struct(head);
	return ptr;
}
