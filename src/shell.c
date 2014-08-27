#include <stdio.h>
#include <string.h>
#include "brainfuck.h"
#include "shell.h"

#define SHELL_BUFFER_LENGTH 80
#define SHELL_QUIT_CHAR 'q'
#define SHELL_HELP "help\n"

void
show_shell_help ()
{
  const char *help = "q: quit\n"
    "help: show this help\n\n"
    "Brainfuck cheatsheet:\n"
    "\t< >: move the pointer\n"
    "\t+ -: increment/decrement the value at pointer position\n"
    "\t. ,: write/read text to/from stdout/stdin\n" "\t[ ]: start/end loop";
  printf ("%s\n", help);
}

/*
 * Run the interactive shell
 */
int
run_brainfuck_shell ()
{
  CELL *prog_array = bf_create_array (DEFAULT_ARRAY_SIZE);
  CELL *ptr = prog_array;
  char buffer[SHELL_BUFFER_LENGTH];
  while (buffer[0] != SHELL_QUIT_CHAR)
    {
      memset (buffer, '\0', SHELL_BUFFER_LENGTH);
      printf (">>> ");
      fgets (buffer, SHELL_BUFFER_LENGTH, stdin);
      if (strcmp (buffer, SHELL_HELP) == 0)
	{
	  show_shell_help ();
	}
      else
	{
	  ptr = bf_execute (buffer, prog_array, ptr);
	  if (!ptr)
	    {
	      ptr = prog_array;
	    }
	  //print_current_state(prog_array, ptr, DEFAULT_ARRAY_SIZE);
	}
    }
  return 0;
}
