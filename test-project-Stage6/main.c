//Group 27 - Scott Clelland, James Williamson, Claire Higgins, Zoe Pearson, Craig Murray

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "shell.c"
#define clear() printf("\033[H\033[J")

int main() { //start of main class
    clear();
    char *hist[HISTORY_COUNT] = {NULL};
    int current = 0;
    //char array to store userinput
    char userInput[512];

    //gets original path when shells loaded
    char *originalPath = getenv("PATH");
    //sets path to home of user
    setToHome();


    FILE *file = fopen(".hist_list", "r"); //open up and read from .hist_list file

    if (file == NULL) {// start of if statement
        printf("Error. Failed to open file.\n");
        for (int i = current; i < HISTORY_COUNT; i++) {
            hist[i] = NULL;
        }
    } //end of if statement

    else {// start of else statement

    char line[BUFFER];

    while (fgets(line, BUFFER, file) && current < 20) {
        if (strcmp(line, "\n") != 0) {
            hist[current] = strdup(strtok(line, "\n"));
            current = (current + 1) % HISTORY_COUNT;
        }
    }

    if (current == 20){
        current = (current - 1) % HISTORY_COUNT;
    }

    }//end of else statement
    fclose(file); //close file


    while (1) {//start of while loop

        //while loop is used to get user's input and to validate it
        //i.e. allowing the program to exit, when the user enters certain commands

        printf("C:/ >");

        //checks for ctrl - D
        if ((fgets(userInput, 512, stdin) == NULL)) {
            restorePath(originalPath);
            printf("Exiting\n");

            break;
        }

        //checks if user has entered exit command
        if (strcmp(userInput, "exit\n") == 0 || strcmp(userInput, "Exit\n") == 0) {
            restorePath(originalPath);
            printf("Exiting\n");
            break;
        }
        char temp[512];
        strcpy(temp,userInput);

        //**ptr used to point to char array pointer returned by function stringParser (tokens)
        char **ptr = stringParser(userInput);

        if(strcmp(ptr[0],"!") != 0 && strcmp(ptr[0], "\n") != 0 && strcmp(ptr[0],"!!") != 0 ) {
            hist[current] = strdup(temp);
            current = (current + 1) % HISTORY_COUNT;
        }
        current = checkInput(ptr, hist, current);


    }//end of while loop
    saveHistory(hist, current);
    clear_history(hist);

    //terminates any remaining child processes
    kill(0, SIGINT);
    kill(0, SIGKILL);
    restorePath(originalPath);
    return 0;
} // end of main class


