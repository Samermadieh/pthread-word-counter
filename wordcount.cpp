#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <fstream>
#include <cctype>
#include <iostream>
using namespace std;

typedef struct {
    long *CurrentStatus;
    long InitialValue;
    long TerminationValue;
} PROGRESS_STATUS;



//Progress monitor function:

void * progress_monitor(void * arg){
    
    PROGRESS_STATUS * currStatus = (PROGRESS_STATUS*) arg;
    float fileSize = currStatus->TerminationValue - currStatus->InitialValue;
    int currDash= 0;
    int dashCount = 0;

    while (currDash < 50){
        currDash = (*(currStatus->CurrentStatus) / fileSize * 50);

        if (currDash > dashCount){
            dashCount = currDash;
            cout << "\r";
            for(int i = 0; i < dashCount; i++){
                if((i + 1) % 10 == 0){
                    cout << '+';
                }
                else {
                    cout << '-';
                }
            }
            cout.flush();
        }
    }

    pthread_exit(0);

    return NULL;
}



//Wordcount function:

long wordcount(char* name){

    long byteCount = 0;
    long wordCount = 0;
    char byte;
    char preByte = ' ';

    struct stat stats;
    int rc = stat(name, &stats);
    long length = rc == 0 ? stats.st_size : -1;

    if(length == 0) {
        return 0;
    }

    PROGRESS_STATUS status;
    status.CurrentStatus = &byteCount;
    status.InitialValue = 0;
    status.TerminationValue = length;

    pthread_t thread;

    pthread_create(&thread, NULL, progress_monitor, (void *)&status);

    //Count bytes and words

    ifstream file(name);

    while(file.get(byte)){
        byteCount++;
        if(isspace(byte)){
            preByte = ' ';
            continue;
        }
        else {
            if(isspace(preByte)){
                wordCount++;
            }
            preByte = 'a';
        }
    }

    pthread_join(thread, NULL);

    return wordCount;
}



//Main function:

int main(int argc, char* argv[]){

    //Checking if file exists/is entered and opens without errors.
    FILE *file;

    if(argc == 1){
        printf("no file specified\n");
        return 0;
    }

    if((file = fopen(argv[1], "r"))){
        fclose(file);
    } else {
        printf("could not open file\n");
        return 0;
    }

    char* fileName = argv[1];

    long numWords = wordcount(fileName);

    printf("\nThere are %ld words in %s.\n", numWords, fileName);

    return 0;
}

