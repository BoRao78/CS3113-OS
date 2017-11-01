/*
	environ - skeleton program displaying environment

	usage:
	
		environ

		dispaly environment with each name, value pair on a separate line
*/

#include <stdio.h>
#include <stdlib.h>

extern char **environ;					//environment array


int environment(){

	char ** env = environ;

	while(*env) printf("%s\n", *env++);		//step through environment

}
