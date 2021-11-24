/*
 * specialcharcount.c - this file implements the specialcharcount function.
 */


#include <stdio.h>
#include <dirent.h>
#include <ctype.h> 
#include <string.h>
#include "count.h"

void specialcharcount(char *path, char *filetowrite, long charfreq[]) {

    // TO-DO: please fill your code here ...
    // pointer to a file
    struct dirent *entry;
    char txt[] = ".txt";
    int txt_len = strlen(txt);
    char * p;
    char ch;
    FILE * stream;
    char file_path[512];
    int i;
    char specials[] = {
        ',', '.', ':', ';', '!'
    };
    int name_len;

    // pointer to the directory
    DIR *dr = opendir(path);

    // if the path is not a directory
    if (dr == NULL) {
        printf("Could not open %s\n", path);
        return;
    }


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
                /*
                 * 2) Read all files in the filelist one by one and count the
                 *  frequency of each alphabet letter only (a - z). The array 
                 *   long alphabetfreq[] always has the up-to-date frequencies 
                 * of alphabet letters counted so far. 
                 * If the letter is upper case, convert
                 *   it to lower case first.
                 */

                sprintf(file_path, "%s/%s", path, entry->d_name);

                stream = fopen(file_path, "r");

                // file cannot be opened for reading
                if (stream == NULL) {
                    // do nothing
                    // silently ignore it 
                } else {
                    // get each character from file
                    while ((ch = fgetc(stream)) != EOF) {
                        // ch is a letter                        
                        for (i = 0; i < SPECIALCHARSIZE; i++) {
                            if (ch == specials[i]) {
                                charfreq[i]++;
                                break;
                            }
                        }
                    }
                    // close reading stream
                    fclose(stream);
                } // end reading file
            } // end file ending with .txt
        } // end file's name having more than 4 characters length
        //  printf("%s\n", entry->d_name);        
    } // end getting files from directory

    // close the pointer to the directory
    closedir(dr);

    //  3) Write the result in the output file

    // open file
    stream = fopen(filetowrite, "w");

    // cannot open the file for writing 
    if (stream == NULL) {
        // ignore it 
    } else {
        // iterate over the array of letter
        for (i = 0; i < SPECIALCHARSIZE; i++) {
            // write data to file             
            fprintf(stream, "%c -> %ld\n", specials[i], charfreq[i]);
        }
        // close the file
        fclose(stream);
    }

}
/**
  The specialcharcount function counts the frequency of the following 5 special characters:
  ','   '.'   ':'    ';'    '!'
   
  in all the .txt files under directory of the given path and write the results to a file named as filetowrite.
  
  Input: 
  
      path - a pointer to a char string [a character array] specifying the path of the directory; and
      filetowrite - a pointer to a char string [a character array] specifying the file where results should be written in.
      alphabetfreq - a pointer to a long array storing the frequency of the above 5 special characters
      
      charfreq[0]: the frequency of ','
      charfreq[1]: the frequency of '.'
      charfreq[2]: the frequency of ':'
      charfreq[3]: the frequency of ';'
      charfreq[4]: the frequency of '!'

  
  Output: a new file named as filetowrite with the frequency of the above special characters written in
  
  Steps recommended to finish the function:
  1) Find all the files ending with .txt and store in filelist.
  2) Read all files in the filelist one by one and count the frequency of each alphabet letter only (a - z). The array 
  long alphabetfreq[] always has the up-to-date frequencies of alphabet letters counted so far. If the letter is upper case, convert
  it to lower case first. 
  3) Write the result in the output file: filetowrite in following format: 
  
     character -> frequency
     
     example:
     , -> 20
     . -> 11
     : -> 20
     ; -> 11
     ! -> 12     
     
  Assumption:  
  1) You can assume there is no sub-directory under the given path so you don't have to search the files 
  recursively.    
  2) Only .txt files are counted and other types of files should be ignored.
  
 */

