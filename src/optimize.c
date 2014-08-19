#include <string.h>
#include <stdlib.h>
#include "optimize.h"

/*
 * Remove all non-brainfuck characters from the given program string.
 * Alters the current file_string -> very efficient
 */
char* bf_minimize_file(char* file_string)
{
	char* valid_chars = "<>.,+-[]";
	int i;
	int insert_pos = 0;
	for(i=0; i<strlen(file_string); i++){
		char current_char = file_string[i];
		if(strchr(valid_chars, current_char)){
			//character is valid
			file_string[insert_pos] = current_char;
			insert_pos++;
		}
	}
	//add trailing null-byte
	file_string[insert_pos] = '\0';
	//reduce heap memory
	file_string = realloc(file_string, insert_pos+1);
	return file_string;
}
