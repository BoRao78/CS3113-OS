/*
CS 3113 Project 1b
Bo Rao   113417729
Oct./31/2017

Extension of project 1a.
Accept command line input and execute both Internal or External Commands/Aliases.
Can deal with indefinite parameters.

Internal commands:
	
	cd - change the directory of process and environment variables.
	clr - clears the screen
	dir <directory> - list the current directory centents in <directory>
	environ - list all the environment strings
	quit - exits from the program

External commands:
	
	pass command to OS and execute, accepte uncertain number of parameters, use fork-exec() 
instead of system(). 

All commands can realize I/O redirection forms as "command arg1 arg2 ... < inputfile > outputfile" 

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;		//environment array

#define MAX_BUFFER 1024         //max line buffer
#define MAX_ARGS 64             //max # args
#define SEPARATORS " \t\n"      //token sparators

int main(int argc, char ** argv){
	
	char buf[MAX_BUFFER];
	char * args[MAX_ARGS];
	char * argvs[MAX_ARGS];
	char ** arg;
	char prompt[MAX_ARGS] = "==>";
	int dont_wait = 0;

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
				argvs[0] = "clear";
				argvs[1] = NULL;
				pid_t pid = 0;	
				switch (pid = fork ()) {
					case -1:
						fprintf(stderr, "fork error\n");
					case 0:			// child
						execvp("clear", argvs);
						fprintf(stderr, "exec error\n");
					default:		// parent
						if (!dont_wait)
							wait(NULL);
				}
				continue;
			}
// change current directory and set and change environmrnt strings

			if(!strcmp(args[0], "cd")){	//"cd" command

				//check and change directory
				arg = args;	
				char *current_dir = (char *)malloc(MAX_ARGS);
				char *dir = (char *)malloc(MAX_ARGS);
				if(!getcwd(current_dir, MAX_ARGS)){
					fprintf(stdout, "Error Current Directory!\n");
					continue;
					}
				fprintf(stdout, "Previous Directory: %s\n",current_dir);
				*arg++;
				dir = strcpy(dir, *arg);
				if(chdir(dir) == -1){
					fprintf(stdout, "Error Current Directory!\n");
					continue;
					}
				fprintf(stdout, "Current Directory: %s\n",getcwd(dir, MAX_ARGS));
				
				//change PWD environ
				char *OLDPWD = (char *)malloc(MAX_ARGS);
				strcpy(OLDPWD, getenv("PWD"));
				setenv("OLDPWD", OLDPWD, 1);	
				setenv("PWD", dir, 1);			
	
				//put name in the shell prompt
				strcpy(prompt, "==>");
				strcat(prompt, dir);
				strcat(prompt, " $ ");
	
				free(current_dir);free(dir);free(OLDPWD);
				continue;
			}
			if(!strcmp(args[0], "dir")){	//"dir" command
				arg = args;
				int flagr = 0;
				int flagw = 0;
				int flagdir = 0;
				char *rfile = (char *)malloc(MAX_ARGS);
				char *wfile = (char *)malloc(MAX_ARGS); 
				argvs[0] = "ls";
				argvs[1] = "-al";			
				
				*arg++;
				while(*arg){
					if(!strcmp(*arg, "<")){
						*arg++;
						flagr = 1;
						strcpy(rfile, *arg);
						*arg++;
						continue;
						}
					if(!strcmp(*arg, ">")){
						*arg++;
						flagw = 1;
						strcpy(wfile, *arg);
						*arg++;
						continue;
						}
					if(!strcmp(*arg, ">>")){
						*arg++;
						flagw = 2;
						strcpy(wfile, *arg);
						*arg++;
						continue;
						}
					
					argvs[2] = *arg;
					argvs[3] = NULL;
					flagdir = 1;
					*arg++;
					}				


				if(!flagdir)				
					argvs[2] = NULL;

				pid_t pid = 0;	
				switch (pid = fork ()) {
					case -1:
						fprintf(stderr, "fork error\n");
					case 0:			// child
						if(flagr) {FILE *fr = freopen(rfile, "r", stdin);}
						if(flagw == 1) {FILE *fw = freopen(wfile, "w", stdout);}
						if(flagw == 2) {FILE *fw = freopen(wfile, "a", stdout);}

						execvp("ls", argvs);
						fprintf(stderr, "exec error\n");
					default:		// parent
						if (!dont_wait)
							wait(NULL);
				}
				free(rfile);free(wfile);
				continue;
			}
			
			if(!strcmp(args[0], "environ")){	//"environ" command
				arg = args;
				int flagr = 0;
				int flagw = 0;
				char *rfile = (char *)malloc(MAX_ARGS);
				char *wfile = (char *)malloc(MAX_ARGS); 
				*arg++;
				while(*arg){
					if(!strcmp(*arg, "<")){
						*arg++;
						if(!*arg) break;
						flagr = 1;
						strcpy(rfile, *arg);
						*arg++;
						continue;
						}
					if(!strcmp(*arg, ">")){
						*arg++;
						if(!*arg) break;
						flagw = 1;
						strcpy(wfile, *arg);
						*arg++;
						continue;
						}
					if(!strcmp(*arg, ">>")){
						*arg++;
						if(!*arg) break;
						flagw = 2;
						strcpy(wfile, *arg);
						*arg++;
						continue;
						}
					}				
				pid_t pid = 0;	
				switch (pid = fork ()) {
					case -1:
						fprintf(stderr, "fork error\n");
					case 0:			// child
						if(flagr) {FILE *fr = freopen(rfile, "r", stdin);}
						if(flagw == 1) {FILE *fw = freopen(wfile, "w", stdout);}
						if(flagw == 2) {FILE *fw = freopen(wfile, "a", stdout);}
						char ** env = environ;
						while(*env) printf("%s\n", *env++);//step through environment
						exit(0);
					default:		// parent
						if (!dont_wait)
							wait(NULL);
				}
			
				free(rfile);free(wfile);
				continue;
			}

			if(!strcmp(args[0], "quit")){	//"quit" command
				break;			//break out of 'while' loop
			}

			// else pass command onto OS(if failed to execute, print them out)
			arg = args;
			int flagr = 0;
			int flagw = 0;
			char *rfile = (char *)malloc(MAX_ARGS);
			char *wfile = (char *)malloc(MAX_ARGS); 
			arg++;
			while(*arg){
				if(!strcmp(*arg, "<")){
					arg++;
					flagr = 1;
					strcpy(rfile, *arg);
					}
				if(!strcmp(*arg, ">")){
					arg++;
					flagw = 1;
					strcpy(wfile, *arg);
					}
				if(!strcmp(*arg, ">>")){
					arg++;
					flagw = 2;
					strcpy(wfile, *arg);
					}
				arg++;
				}		
			arg = args;
			int n = 0;	//number of input
			while(*arg++) n++;
			if(flagr && flagw) args[n - 4] = NULL;
			if(flagr || flagw) args[n - 2] = NULL;
			pid_t pid = 0;	
			switch (pid = fork ()) {
				case -1:
					fprintf(stderr, "fork error\n");
				case 0:			// child
					if(flagr) {FILE *fr = freopen(rfile, "r", stdin);}
					if(flagw == 1) {FILE *fw = freopen(wfile, "w", stdout);}
					if(flagw == 2) {FILE *fw = freopen(wfile, "a", stdout);}
					
					execvp(args[0], args);
					fprintf(stderr, "exec error\n");//error to execute
					arg = args;
					while(*arg) fprintf(stdout, "%s ", *arg++);
					fputs("\n", stdout);		
				default:		// parent
					if (!dont_wait)
						wait(NULL);
				}
			free(rfile);free(wfile);
			}
		}
	}
}

