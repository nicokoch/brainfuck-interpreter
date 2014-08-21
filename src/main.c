#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "brainfuck.h"
#include "optimize.h"
#include "shell.h"

void show_help()
{
	const char *help_text = "Brainfuck interpreter by Nicolas Koch\n"
	    "Syntax: brainfuck-interpreter [-vh] bf-filename\n"
	    "Without arguments, this program will run in interactive mode. In this case type \"help\" for more information.\n"
	    "Options:\n"
	    "\t-h: show this help\n"
	    "\t-v: show version information\n" "\t-m: minimize brainfuck file";
	printf("%s", help_text);
}

void show_version()
{
	printf("VERSION\n");
}

int main(int argc, char **argv)
{
	char *opt_filename = NULL;
	int opt_flag_minimize = 0;
	int opt_flag_interactive = 0;

	int index;
	int c;

	opterr = 0;
	if (argc == 1) {	//no arguments -> run interactive
		opt_flag_interactive = 1;
	}
	while ((c = getopt(argc, argv, "hvm")) != -1) {
		switch (c) {
		case 'h':
			show_help();
			return 0;
		case 'v':
			show_version();
			return 0;
		case 'm':
			opt_flag_minimize = 1;
			break;
		case '?':
			if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n",
					optopt);
			else
				fprintf(stderr,
					"Unknown option character `\\x%x'.\n",
					optopt);
			return 1;
		}
	}

	for (index = optind; index < argc; index++) {
		if (index == argc - 1) {
			opt_filename = argv[index];
		}
	}
	if (!opt_filename && !opt_flag_interactive) {
		fprintf(stderr,
			"No filename given. Try -h for more information\n");
		return 1;
	}

	if (opt_flag_minimize) {	//minimize the file
		char *program_string = bf_read_file(opt_filename);
		program_string = bf_minimize_file(program_string);
		printf("%s\n", program_string);
		free(program_string);
	} else if (opt_flag_interactive) {	//run in interactive mode
		run_brainfuck_shell();
	} else {		//execute the file
		char *program_string = bf_read_file_and_minimize(opt_filename);
		if (!program_string) {	//error
			return 1;
		}
		CELL *prog_array = bf_create_array(DEFAULT_ARRAY_SIZE);
		CELL *ptr = prog_array;
		ptr = bf_execute(program_string, prog_array, ptr);
		bf_destroy_array(prog_array);
		free(program_string);
		if (!ptr) {	//error
			fprintf(stderr, "An error occured during execution\n");
			return 1;
		}
	}
	return 0;
}
