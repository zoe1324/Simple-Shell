//header files
#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

//defining section
#define HISTORY_COUNT 20
#define BUFFER 512
#define clear() printf("\033[H\033[J")

char **stringParser(char *userInput)

    //used to separate the user's input into multiple different parts
    //so it can be used when executing commands

{//start of stringParser method
    strtok(userInput, "\n");
    char *token = strtok(userInput, " \t |<> &;");
    char **tokens = malloc(50* sizeof(char*));

    int c = 0;
    while (token != NULL)
    {
        tokens[c++] = token;
        token = strtok(NULL, " ");
    }
    return tokens;
}//end of stringParser method

int executeExternal(char **userInput)

    //used to execute linux commands

{//start of executeExternal method
    //initialises path variables
    char *basePath = "/bin/";
    int status;
    //allocates memory for char ptr
    char *temp = malloc(sizeof(char*));

    //sets temp to /bin/ + users entered command
    strcpy(temp, basePath);
    strcat(temp, userInput[0]);
    char *cmd = temp;

    //creates parent &child processes
    pid_t pid;
    pid_t cpid = fork();

    //if fork fails then child process is killed
    if (cpid == -1)
    {
        perror("fork has failed");
        kill(cpid, SIGTERM);
    }
    else if (cpid == 0)
    {
        //printf("Child process executing:\n");
        //checks if user has entered an external command and either displays error if so
        if (execvp(cmd, userInput) == -1)
        {
            printf("Error: %s command not found.\n", userInput[0]);
        }
        else
        {
            //otherwise executes
            execvp(cmd, userInput);
        }
    }
    else if (cpid > 0)
    {
        //if no child process terminates then wait fails
        if ((pid = wait(&status)) < 0)
        {
            perror("wait failed");
        }
    }

    //printf("Parent finished\n");
    return 0;

}//end of executeExternal method

void setToHome(){//start of setToHome method

    //used to revert user's path back to the home path

    char *homePath = getenv("HOME");
    //printf("%s\n", originalPath);
    //printf("%s\n", homePath);
    setenv("PATH", homePath, 1);
    char *newPath = getenv("PATH");
    //printf("%s\n", newPath);
    chdir(homePath);

}//end of setToHome method

void changePath(char *pathString)
{//start of changePath method


    //sets path to string entered by user

    setenv("PATH", pathString, 1);

}//end of changePath method

char *returnPath(){//start of returnPath method

    //gets current set path and returns

    char *currentPath = getenv("PATH");
    return currentPath;

}//end of returnPath method

void restorePath(char *originalPath)
{//start of restorePath method


    //sets path to original path when shell started up & displays

    setenv("PATH", originalPath, 1);
    printf("Restoring original path\n");
    printf("%s\n", getenv("PATH"));

}//end of restorePath method

void changeDirectory(char** tokStringArr){ //start of changeDirectory method

    //used to invoke the linux 'cd' command

    char error[150];

    if (tokStringArr[1] != NULL && tokStringArr[2] == NULL){
        int i = 1;

        while(tokStringArr[i] != NULL){
            if (chdir(tokStringArr[i]) != 0){

                fprintf(stderr, "cd: %s: ", tokStringArr[i]);
                perror("");
                break;
            }
            i++;
        }

    } else if(tokStringArr[2] != NULL) {

        fprintf(stderr, "cd should only take a single argument\n");
    }else if (tokStringArr[1] == NULL){
        setToHome();
    }

}//end of changeDirectory method


int userHistory(char *userHist[], int index){ //start of userHistory method

    //displays the user's history


    int i = index;
    int hist_index = 1;

    do {// start of do while loop
        //displays history and stores it as a circular array
        //allows program to keep storing history over 20 items
        //does this by removing first element of the array when the user adds when they array is full


        if (userHist[i]){
            printf("%d, %s\n", hist_index, userHist[i]);
            hist_index++;
        }
        i = (i+1) % 20;
    }while (i != index); //end of do while loop

    return 0;

}// end of userHistory method

int clear_history(char *hist[]){//start of clear_history method

    //method is used for clearing history
    //and clearing memory

    int i;

    for (i = 0; i < HISTORY_COUNT; i++) {
        hist[i] = NULL;
    }

    return 0;

}//end of clear_history method

void checkHistory(int n, char *hist[], int curr) {//start of checkHistory method
    //used to validate history commands
    // such as invoking commands from history

    if (n > 20) { //if used to check if user input is greater than 20
        printf("Error, index exceeds history size (20)\n");
    } //end of if statement

    else if (n < -20){// else if used to check if user input is less than minus 20
        printf("Error, negative number is out of range\n");
    }//end of else if

    else if(hist[0] == NULL) { //check if history is empty
        printf("History is empty!\n");

    }//end of else if

    else if (hist[19] != NULL && curr != 19) { //if history is at full capacity (therefore current could be 0 with the rest of history not null)
       //temp variable for current
        int i = (curr);
        //variable used to count number of moves made
        int x = 0;

        //checks if number is positive or negative & calculates number of times i needs to be calculated
        //to get to such index
        if (n > 0) {
            n = n - 1;
        } else if(n < 0) {
            n = 20 + n;
        }

        //loops to calculate i
        while (x != n) {
            if (hist[i] != NULL) {
                i = (i + 1) % 20;
                x++;
            }
        }

        //if no value is stored error message displays
        if (hist[i] == NULL) {
            printf("error - no command stored at entered index");
        }

        else {
            //otherwise executes command
            char **ptr = stringParser(hist[i]);
            checkInput(ptr, hist, curr);
        }
    }

    else if(hist[19] == NULL){ //if history is not full (i.e. null values between index 0 and 19)

        //initial counter set to -1
        int i = -1;

        //only if history has values stored
        if(hist[0] != NULL) {
            //i is then set to 0
            i = 0;
            //counts number of values stored
            while (hist[i] != NULL) {
                i++;
            }
            //calculates n if positive or negative
            if (n < 0) {
                n = i + n;
            } else {
                n = n - 1;
            }
            //if entered n is smaller than 20, value is not null and index is not out of bounds it executes command
            if (n < HISTORY_COUNT && hist[n] != NULL && n < i) {
                char **ptr = stringParser(hist[n]);
                checkInput(ptr, hist, curr);
            }

            //otherwise appropriate error messages are displayed
            else if(n >= i){
                printf("Too high an index! No commands are stored at the specified index \n");
            }

            else if(n < 0){
                printf("Negative number entered was out of range. \n");
            }

        }

    }
} //end of checkHistory method





int checkInput(char **ptr, char *hist[], int current){ //start of checkInput method
    //used to get select what to do with the user's input

    if (strcmp(ptr[0], "setpath") == 0 && ptr[1] != NULL && ptr[2] == NULL) { //start of if statement used if user's input is "setpath"
        for (int i = 1; ptr[i + 1] != NULL; i++) {
            strcat(ptr[i], ptr[i + 1]);
        }
        changePath(ptr[1]);
    }//end of if statement

    else if (strcmp(ptr[0], "setpath") == 0 && ptr[1] == NULL){//start of else if statement used to check setpath has correct amount of parameters
        fprintf(stderr,"too few parameters to setpath.\n");
    }//end of else if

    else if (strcmp(ptr[0], "setpath") == 0 && ptr[2] != NULL){//start of else if statement used to check setpath doesnt have too many parameters
        fprintf(stderr,"too many parameters to setpath.\n");
    }//end of else if statement

    else if (strcmp(ptr[0], "getpath") == 0 && ptr[1] == NULL)
    {
        printf("%s\n", returnPath());
    }

    else if (strcmp(ptr[0], "getpath") == 0 && ptr[1] != NULL)
    {
        printf("Error: getpath only takes 1 parameter\n");
    }
    else if (strcmp(ptr[0], "clear") == 0)
    {//start of else if statement used to clear screen
        clear();
    }//end of else if statement

    else if (strcmp(ptr[0],"cd") == 0){//start of else if statement used to invoke 'cd' command
        changeDirectory(ptr);
    }//end of else if

    else if (strcmp(ptr[0], "history") == 0 && ptr[1] != NULL){//
        printf("Error! history does not take any parameters\n");
    }

    else if (strcmp(ptr[0], "history") == 0) {//start of history else if
        userHistory(hist, current);

    }//end of history else if

    else if (strcmp(ptr[0], "hc") == 0) {//start of hc else if
        clear_history(hist);
        current = 0;
    }//end of hc else if

    else if (strcmp(ptr[0],"!") == 0) { //start of "!" else if, part of invoking commands from history
        if (ptr[1] != NULL && ptr[2] == NULL) {

            int i = atoi(ptr[1]);

            if( i == 0){
                printf("Error, ! only takes positive or negative integers as parameters!\n");

            } else {
                checkHistory(i, hist, current);
            }

        }

        else if(ptr[1] == NULL) {
            printf("Not enough parameters to execute ! \n");
        }

        else if (ptr[2] != NULL){
            printf("Too many parameters to execute ! \n");
        }
    } //end of "!" else if

    else if(strcmp(ptr[0], "!!") == 0){//start of "!!" else if
        if(ptr[1] != NULL){
            printf("Too many parameters to execute !! \n");
        } else {

            if(hist[19] == NULL) {
                checkHistory(current, hist, current);
            } else {
                checkHistory(20,hist,current);
            }
        }
    } //end of "!!" else if

    else {
        if (strcmp(ptr[0], "\n") != 0) {
            //executes command entered by user
            executeExternal(ptr);
        }
    }
    return current;

}//end of checkInput method



void saveHistory(char *hist[], int index) {//start of saveHistory method

    //Used to save user's history

    FILE *fp;
    setToHome();
    fp = fopen(".hist_list", "w"); //open and write to .hist_list file, allows user's history to be saved for later

    int i = index;
    int hist_index = 1;
    do {

        if (hist[i]){
            fprintf(fp, "%s\n", hist[i]);
            hist_index++;
        }
        i = (i+1) % 20;
    }while (i != index);

    fclose(fp); // closing file

}//end of saveHistory method

