/*
CS 3113 Project 1a
Bo Rao   113417729
Oct./9/2017

Accept command line input and execute both Internal or External Commands/Aliases.
Can deal with indefinite parameters.

Internal commands:
	
	clr - clears the screen
	dir <directory> - list the current directory centents in <directory>
	environ - list all the environment strings
	quit - exits from the program

External commands:
	
	pass command to OS and execute, accepte uncertain number of parameters

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "environ.h"

#define MAX_BUFFER 1024         //max line buffer
#define MAX_ARGS 64             //max # args
#define SEPARATORS " \t\n"      //token sparators

int main(int argc, char ** argv){
	
	char buf[MAX_BUFFER];
	char * args[MAX_ARGS];
	char ** arg;
	char * prompt = "==>";

/* keep reading input until "quit" command or eof of redirected input */

	while(!feof(stdin)){

// get command line from input
	
	fputs(prompt, stdout);
	if(fgets(buf, MAX_BUFFER, stdin)){
		
// tokenize the input into args array
		
		arg = args;
		*arg++ = strtok(buf, SEPARATORS);	//tokenize input
		while(*arg++ = strtok(NULL, SEPARATORS));
							//last entry will be NULL

		if(args[0]){				//if there's anything there
			
// check for internal/external command
			
			if(!strcmp(args[0], "clr")){	//"clr" command
				system("clear");			
				continue;
			}

			if(!strcmp(args[0], "dir")){	//"dir" command
				arg = args;
				char *dir = (char *)malloc(MAX_ARGS);
				dir = strcat(dir, "ls -al ");
				*arg++; 
				while(*arg) {
					dir = strcat(dir, *arg++);
					dir = strcat(dir, " ");
				}
				//fprintf(stdout, "%s\n",dir);
				system(dir);
				free(dir);		
				continue;
			}
			
			if(!strcmp(args[0], "environ")){	//"environ" command
				environment();			
				continue;
			}

			if(!strcmp(args[0], "quit")){	//"quit" command
				break;			//break out of 'while' loop
			}

			// else pass command onto OS(if failed to execute, print them out)
			
			arg = args;
			char *execute = (char *)malloc(MAX_ARGS); 
			while(*arg) {
				execute = strcat(execute, *arg++);
				execute = strcat(execute, " ");
			}
			//fprintf(stdout, "%s\n",execute);
			if(system(execute)){	
				arg = args;
				while(*arg) fprintf(stdout, "%s ", *arg++);
				fputs("\n", stdout);		
				}
			free(execute);
			}
		}
	}
}

