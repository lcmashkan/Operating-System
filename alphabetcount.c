/*
 * alphabetcount.c - this file implements the alphabetlettercount function.
 */

#include <stdio.h> 
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include "count.h"

/**
  The alphabetlettercount function counts the frequency of each alphabet letter (a-z, case insensitive) in all the .txt files under
  directory of the given path and write the results to a file named as filetowrite.
  
  Input: 
      path - a pointer to a char string [a character array] specifying the path of the directory; and
      filetowrite - a pointer to a char string [a character array] specifying the file where results should be written in.
      alphabetfreq - a pointer to a long array storing the frequency of each alphabet letter from a - z:
      alphabetfreq[0]: the frequency of 'a'
      alphabetfreq[1]: the frequency of 'b'
         ... ...
      alphabetfreq[25]:the frequency of 'z'

  
  Output: a new file named as filetowrite with the frequency of each alphabet letter written in
  
  Steps recommended to finish the function:
  1) Find all the files ending with .txt and store in filelist.
  2) Read all files in the filelist one by one and count the frequency of each alphabet letter only (a - z). The array 
  long alphabetfreq[] always has the up-to-date frequencies of alphabet letters counted so far. If the letter is upper case, convert
  it to lower case first. 
  3) Write the result in the output file: filetowrite in following format: 
  
     letter -> frequency
     
     example:
     a -> 200
     b -> 101
     ... ...
     
  Assumption:  
  1) You can assume there is no sub-directory under the given path so you don't have to search the files 
  recursively.    
  2) Only .txt files are counted and other types of files should be ignored.
  
 */
void alphabetlettercount(char *path, char *filetowrite, long alphabetfreq[]) {

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
        for (i = 0; i < ALPHABETSIZE; i++) {
            // write data to file 
            ch = i + 'a';
            fprintf(stream, "%c -> %ld\n", ch, alphabetfreq[i]);
        }        
        // close the file
        fclose(stream);
    }
}
