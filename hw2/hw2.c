#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    //Naive Error Check
    if(argc != 3){
        fprintf(stderr, "Two parameters an int and a char.\n");
        exit(-1);
    }

    //Get the size of the diamond
    int num = atoi(argv[1]);

    if(num < 1 || num > 15){
        fprintf(stderr, "The number must be between 1 and 15.\n");
    }

    char *k;
    //Set the character k
    k = argv[2];

    for(int i = 1; i <= num; i++ ){
        char* line = (char *)malloc(num*2);
        for(int j = 1; j <= num*2 - 1; j++ ){
            if(j <= num - i || j >= num + i)
                line[j-1] = ' ';
            else
                line[j-1] = k[0];
        }
        line[num*2 - 1] = '\0';
        fprintf(stdout, "%s\n",line);
        free(line);
    }
    for(int i = num - 1; i > 0; i-- ){
        char* line = (char *)malloc(num*2);
        for(int j = 1; j <= num*2 - 1; j++ ){
            if(j <= num - i || j >= num + i)
                line[j-1] = ' ';
            else
                line[j-1] = k[0];
        }
        line[num*2 - 1] = '\0';
        fprintf(stdout, "%s\n",line);
        free(line);
    }


    return 0;
}