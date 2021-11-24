/*
 * alphabetcountmulthreads.c - this file implements the alphabetcountmulthreads function.
 */
#define _GNU_SOURCE // for running alphasort ini c99 mode

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include "count.h"

/**
  The alphabetcountmulthreads function counts the frequency of each alphabet letter (a-z, case insensitive) in all the .txt files under
  directory of the given path and write the results to a file named as filetowrite. Different with programming assignment#0, you need to implement the program using mutithreading.
  
  Input: 
      path - a pointer to a char string [a character array] specifying the path of the directory; and
      filetowrite - a pointer to a char string [a character array] specifying the file where results should be written in.
      alphabetfreq - a pointer to a long array storing the frequency of each alphabet letter from a - z, which should be already up-to-date;
      num_threads - number of the threads running in parallel to process the files
      
       
  Output: a new file named as filetowrite with the frequency of each alphabet letter written in
  
  Requirements:
1)	Multiple threads are expected to run in parallel to share the workload, i.e. suppose 3 threads to process 30 files, then each thread should process 10 files;
2)	When a thread is created, a message should be print out showing which files this thread will process, for example:
Thread id = 274237184 starts processing files with index from 0 to 10!
3)	When a file is being processed, a message should be print out showing which thread (thread_id = xxx) is processing this file, for example:
Thread id = 265844480 is processing file input_11.txt
4)	When a thread is done with its workload, a message should be print out showing which files this thread has done with work, for example:
      Thread id = 274237184 is done !
5)	The array: long alphabetfreq[ ] should always be up-to-date, i.e. it always has the result of all the threads counted so far.  [You may need to use mutexes to protect this critical region.]


You should have the screen printing should be similar as follows:

 Thread id = 274237184 starts processing files with index from 0 to 10!
 Thread id = 265844480 starts processing files with index from 11 to 22!
 Thread id = 257451776 starts processing files with index from 23 to 31!

 Thread id = 265844480 is processing file input_11.txt
 Thread id = 257451776 is processing file input_22.txt
 Thread id = 274237184 is processing file input_00.txt
  � � 

 Thread id = 274237184 is done !
 Thread id = 265844480 is done !
 Thread id = 257451776 is done !

 The results are counted as follows:
 a -> 2973036
 b -> 556908
 c -> 765864
 �  � 
 */



void alphabetcountmulthreads(char *path, char *filetowrite, long alphabetfreq[], int num_threads) {
    int file_num = countTxtFile(path);
    int i;
    FILE * stream;
    char ch;

    if (file_num <= 0) {
        return;
    }

    if (file_num < num_threads) {
        num_threads = file_num;
    }
    char ** file_paths = (char**) calloc(file_num, sizeof (char *));
    char ** file_names = (char**) calloc(file_num, sizeof (char *));
    getFileNames(path, file_names, file_paths);

    multhreads(file_num, file_names, file_paths, alphabetfreq, num_threads);


    // free allocated memory
    for (i = 0; i < file_num; i++) {
        free(file_paths[i]);
        free(file_names[i]);
    }
    free(file_paths);
    free(file_names);

    // open file
    stream = fopen(filetowrite, "w");

    // cannot open the file for writing 
    if (stream == NULL) {
        // ignore it 
    } else {
        // iterate over the array of letter
        for (i = 0; i < ALPHABETSIZE; i++) {
            // write data to file 
            ch = i + 'a';
            fprintf(stream, "%c -> %ld\n", ch, alphabetfreq[i]);
        }
        // close the file
        fclose(stream);
    }
}

void multhreads(int file_num, char ** file_names, char ** file_paths, long alphabetfreq[], int num_threads) {
    int num_per_thread = file_num / num_threads;

    param para[num_threads];
    pthread_t tid[num_threads];
    pthread_mutex_t lock;
    int thread_running_counter;

    pthread_mutex_init(&lock, NULL);


    int i;
    int start = 0;
    thread_running_counter = 0;
    
    // set up parameters to pass to threads 
    for (i = 0; i < num_threads; i++) {
        para[i].file_paths = file_paths;
        para[i].file_names = file_names;
        para[i].alphabetfreq = alphabetfreq;
        para[i].writer_mutex = &lock;
        para[i].thread_running_counter = &thread_running_counter;
        para[i].num_threads = num_threads;

        // set  up the range of index of files which will be processed per thread
        para[i].start = start;

        start += num_per_thread;
        if (i == num_threads - 1) {
            para[i].end = file_num;
        } else {
            para[i].end = start;
        }
    }

    /// run all thread
    for (i = 0; i < num_threads; i++) {
        pthread_create(&tid[i], NULL, run, &para[i]);
    }


    // join all thread
    for (i = 0; i < num_threads; i++) {
        pthread_join(tid[i], NULL);
        
        // a programming trick to get print out a new line at the end of 
        // the final thread
        if (thread_running_counter == num_threads) {
            // sleep for a while
            usleep(30);
            printf("\n");
            thread_running_counter--;
        }
        printf("Thread id = %lu is done !\n", tid[i]);
    }

    pthread_mutex_destroy(&lock);


    printf("\n");
}

void * run(void *pr) {
    
    // pointer to the data 
    param * para = (param *) pr;
    pthread_t tid = pthread_self();
    
    printf("Thread id = %lu starts processing files with index from %d to %d!\n",
            tid, para->start, para->end - 1);

    // try to add a new line to screen when the final thread starts
    // 
    pthread_mutex_lock(para->writer_mutex);
    // update counter 
    *(para->thread_running_counter) = 1 + (*(para->thread_running_counter));
    // use the counter to decide whether we have to print a new line to screen
    if (*(para->thread_running_counter) == para->num_threads) {
        printf("\n");
    }

    pthread_mutex_unlock(para->writer_mutex);

    long alphabetfreq[ALPHABETSIZE] = {0};
    
    // sleep for 20 millisecond so that all thread id have been printed out 
    // before all threads start to run
    usleep(20);

    char ch;
    char * p;
    FILE * stream;
    int i;
    int j;
    for (i = para->start; i < para->end; i++) {
        stream = fopen(para->file_paths[i], "r");

        // file cannot be opened for reading
        if (stream == NULL) {
            // do nothing
            // silently ignore it 
        } else {
            printf("Thread id = %lu is processing file %s\n",
                    tid, para->file_names[i]);
            // get each character from file
            while ((ch = fgetc(stream)) != EOF) {
                // ch is a alphabet letter
                if (isalpha(ch)) {
                    // convert it to its lower case 
                    ch = tolower(ch);
                    //  up-to-date frequencies of alphabet 
                    // letters counted so far
                    alphabetfreq[(ch - 'a')]++;
                }
            }
            // close reading stream
            fclose(stream);
        } // end reading file
    }

    // lock all reading or writing request 
    pthread_mutex_lock(para->writer_mutex);
    // update the global frequency 
    for (i = 0; i < ALPHABETSIZE; i++) {
        para->alphabetfreq[i] += alphabetfreq[i];
    }
    pthread_mutex_unlock(para->writer_mutex);
    
    pthread_exit(NULL);
}

void getFileNames(char *path, char ** file_names, char ** file_paths) {

    struct dirent **namelist;
    int i, n;
    char txt[] = ".txt";
    int txt_len = strlen(txt);
    char * p;
    char file_path[512];
    int counter, name_len;

    counter = 0;
    
    // get all files in the directory
    // then sort them by their name to get better performance 
    n = scandir(path, &namelist, 0, alphasort);
    if (n < 0)
        perror("scandir error ");
    else {
        
        // iterate over list of name
        for (i = 0; i < n; i++) {
            // try to filter them
            name_len = strlen(namelist[i]->d_name);
            // bad file name
            if (name_len <= txt_len) {
                // do nothing            
            } else {
                p = namelist[i]->d_name + (name_len - txt_len);

                // file's name doesn't end with .txt
                if (strcmp(p, txt) != 0) {
                    // do nothing                
                } else {

                    // copy name
                    // to file path for reading 
                    sprintf(file_path, "%s/%s", path, namelist[i]->d_name);
                    file_paths[counter] = (char *)
                            calloc(strlen(file_path) + 1, sizeof (char));
                    strcpy(file_paths[counter], file_path);

                    // for displaying 
                    // the system gets some trouble while 
                    // splitting file path by / to get file name
                    file_names[counter] = (char *)
                            calloc(strlen(namelist[i]->d_name) + 1, sizeof (char));
                    strcpy(file_names[counter], namelist[i]->d_name);

                    counter++;
                } // end file ending with .txt
            } // end file's name having more than 4 characters length

            // free data of the current file 
            free(namelist[i]);
        }
    }
    free(namelist);
}

int countTxtFile(char *path) {
    struct dirent *entry;
    char txt[] = ".txt";
    int txt_len = strlen(txt);
    char * p;
    int name_len;
    int counter;

    // pointer to the directory
    DIR *dr = opendir(path);

    // if the path is not a directory
    if (dr == NULL) {
        printf("Could not open %s\n", path);
        return 0;
    }

    counter = 0;
    // get files from the directory 
    while ((entry = readdir(dr)) != NULL) {
        // 1) Find all the files ending with .txt and store in filelist.


        name_len = strlen(entry->d_name);
        // bad file name
        if (name_len <= txt_len) {
            // do nothing            
        } else {
            p = entry->d_name + (name_len - txt_len);
            // file's name doesn't end with .txt
            if (strcmp(p, txt) != 0) {
                // do nothing                
            } else {
                counter++;
            } // end file ending with .txt
        } // end file's name having more than 4 characters length

    } // end getting files from directory

    // close the pointer to the directory
    closedir(dr);
    return counter;
}
