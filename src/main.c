#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "brainfuck.h"

void show_help()
{
    printf("HELP\n");
}

void show_version()
{
    printf("VERSION\n");
}

int main(int argc, char** argv) {
    char *opt_filename = NULL;

    int index;
    int c;

    opterr = 0;

    while ((c = getopt (argc, argv, "hv")) != -1)
        switch (c)
        {
        case 'h':
            show_help();
            return 0;
        case 'v':
            show_version();
            return 0;
        case '?':
            if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            return 1;
        }

    for (index = optind; index < argc; index++){
        if (index == argc-1){
            opt_filename = argv[index];
        }
    }
    if(!opt_filename){
        fprintf(stderr, "No filename given. Try -h for more information\n");
        return 1;
    }

    char* program_string = bf_read_file(opt_filename);
    if(!program_string){
        fprintf(stderr, "File not found: %s\n", opt_filename);
        return 1;
    }
    int exec_error = bf_execute(program_string);
    free(program_string);
    if(exec_error){
        fprintf(stderr, "An error occured during execution\n");
        return 1;
    }
    return 0;
}


