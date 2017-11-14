/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2017.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 53-6 */

/* thread_incr_psem.c

   Use a POSIX unnamed semaphore to synchronize access by two threads to
   a global variable.

   See also thread_incr.c and thread_incr_mutex.c.
*/
#include <semaphore.h>
#include <pthread.h>
#include "tlpi_hdr.h"


static int glob = 0;
static sem_t sem;

static void *                   /* Loop 'arg' times incrementing 'glob' */
threadFunc(void *arg)
{
    int loops = *((int *) arg);
    int loc, j;

    for (j = 0; j < loops; j++) {
        if (sem_wait(&sem) == -1)
            errExit("sem_wait");

        loc = glob;
        loc++;
        glob = loc;

        if (sem_post(&sem) == -1)
            errExit("sem_post");
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t *TT;
    int loops, s, num, thread_num;
	
    if(argc > 2){
	loops = getInt(argv[1], GN_GT_0, "num-loops");
	thread_num = getInt(argv[2], GN_GT_0, "num_threads");
    }
    else{
	fprintf(stdout, "Not enough parameters.\n");
	exit(0);
    }
   // loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;

    /* Initialize a semaphore with the value 1 */

    if (sem_init(&sem, 0, 1) == -1)
        errExit("sem_init");

    TT = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
	
    /* Create two threads that increment 'glob' */
    for(num = 0; num < thread_num; num++){
	
	s = pthread_create(&TT[num], NULL, threadFunc, &loops);
	if (s != 0)
		errExitEN(s, "pthread_create");
   
	}

    /* Wait for threads to terminate */

    for(num = 0; num < thread_num; num++){
	
	s = pthread_join(TT[num], NULL);
	if (s != 0)
		errExitEN(s, "pthread_join");
   
	}

    printf("glob = %d\n", glob);
    free(TT);
    exit(EXIT_SUCCESS);
}
