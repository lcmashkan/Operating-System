/*
 This program will fork two child processes running the two programs generated in programming zero in parallel.
 Hint: You mant need to use fork(), exec() family, wait(), exit(), getpid() and etc ...
 
 Requirements:
 
 1) Exactly two child processes are created, one to run testspecial program and the other is to run testalphabet program;
 2) When a program starts to run, print a message to the output screen showing which process (with PID) is running which program, for example:
    "CHILD <16741> process is executing testalphabet program!"
    
 3) When a program is done, the process exits and at the same time, a message should be print to the output screen showing which  process (with PID) is done with the program, for example:
    "CHILD <16741> process has done with testalphabet program !"
    
 4) The messages should match the real execution orders, i.e. when the testspecial program starts/ends, the right message should be print out. So you need to figure out how to get the starting/ending time of each 
 process.
   
   
 The expected screen print out should be similar as follows:
 
 CHILD <16741> process is executing testalphabet program!
 CHILD <16742> process is executing testspecial program!
, -> 745668
. -> 798072
... ...

CHILD <16742> process has done with testspecial program !
a -> 2973036
b -> 556908
... ...

CHILD <16741> process has done with testalphabet program !
 */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>


int main(void) {

    // fill your code here ... 

    pid_t wpid, pid;
    int status;
    // list of executed file
    char arr [2][20] = {
        "testalphabet", "testspecial"
    };

    key_t key;
    int shmid;
    
    // an unique key for the shared memory
    key = 5678;
    
    // get and id for the shared memory
    shmid = shmget(key, sizeof (pid_t) * 2, IPC_CREAT | 0666);
    
    // attach the pointer to the memory
    pid_t * data = shmat(shmid, NULL, 0);

    // make 2 processes 
    // assume that there isn't any error 
    for (int i = 0; i < 2; i++) {
        if ((pid = fork()) == 0) {
            //child code
            printf("CHILD <PID: %d> process is executing %s program!\n",
                    getpid(), arr[i]);
            // store the 
            data[i] = getpid();
            // run the executed file 
            execl(arr[i], arr[i], NULL);
        }
    }

    // waiting for all children
    while ((wpid = wait(&status)) > 0) {
        // figure out which child has stopped
        for (int i = 0; i < 2; i++) {            
            if (data[i] == wpid) {
                // show a message
                printf("CHILD <%d> process has done with %s program !\n",
                        wpid, arr[i]);
            }
        } 
    }

    // detach the shared memory
    shmdt(data); 
    // destroy the shared memory
    shmctl(shmid,IPC_RMID,NULL); 
    
    return 0;

}
