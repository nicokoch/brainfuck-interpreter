#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "brainfuck.h"
#include "optimize.h"

void show_help()
{
    const char* help_text="Brainfuck interpreter by Nicolas Koch\n"
                          "Syntax: brainfuck-interpreter [-vh] bf-filename\n"
                          "Options:\n"
                          "\t-h: show this help\n"
                          "\t-v: show version information\n"
                          "\t-m: minimize brainfuck file";
    printf("%s", help_text);
}

void show_version()
{
    printf("VERSION\n");
}

int main(int argc, char** argv) {
    char *opt_filename = NULL;
    int opt_flag_minimize = 0;
    
    int index;
    int c;

    opterr = 0;

    while ((c = getopt (argc, argv, "hvm")) != -1)
        switch (c)
        {
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
    
    if (opt_flag_minimize){ //minimize the file
        char* program_string = bf_read_file(opt_filename);
        program_string = bf_minimize_file(program_string);
        printf("%s\n",program_string);
        free(program_string);
    }
    else{ //execute the file
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
    }
    
    return 0;
}


