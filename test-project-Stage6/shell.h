//declaring methods
char **stringParser(char *userInput);
int executeExternal(char **userInput);
void setToHome();
void changePath(char *pathString);
char *returnPath();
void restorePath(char *originalString);
void changeDirectory(char** tokStringArr);
int userHistory(char *userHist[], int index);
int clearHistory(char *hist[], int curr);
void checkHistory(int n, char *hist[], int curr);
int checkInput(char **ptr, char *hist[], int current);
void saveHistory(char *hist[], int current);
