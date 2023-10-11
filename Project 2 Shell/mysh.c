#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

const char *invalidArgumentNumber = "Usage: mysh [batch-file]\n";
const char *fileOpenError = "Error: Cannot open file ";
const char *commandPrompt = "mysh> ";
const char *commandNotFoundError = ": Command not found.\n";
const char *redirectionError = "Redirection misformatted.\n";
const char *outputFileError = "Cannot write to file ";
const char *redirectionSymbol = ">";
const int maxCommandLength = 512;
FILE *batchFile = NULL;
FILE *output = NULL;
char *outputFileName = "";


void executable(char **argv, int redirection) {
    pid_t childPID;
    childPID = fork();
    if(childPID == 0) { // This is the child
        if(redirection == 1) {
            int redirectionNum = open(outputFileName, O_WRONLY | O_CREAT);
            dup2(redirectionNum , 1);
        }
        execv(argv[0], argv);
        strcat(argv[0], commandNotFoundError);
        write(STDERR_FILENO, argv[0], strlen(argv[0]));
	    _exit(1);
    }
    else { // if current process is parent
        int status;
        waitpid(childPID, &status, 0);
    }
}

void parse_Command(char *command) {
    char* parsedCommand[maxCommandLength]; // to store the command in the form of array of strings
    char* token;
    if(command[strlen(command)-1] == '\n') {
        char* deleteNewLine = strtok(command, "\n");
        token = strtok(deleteNewLine, " ");
    }
    else {
        token = strtok(command, " ");
    }
    int index = 0; // to iterate through how many arguments in the command
    while(token) {
        parsedCommand[index] = token;
	    index++;
        token = strtok(NULL, " ");
    }
    if(index == 0) { // for empty command line
        return;
    }
    parsedCommand[index] = NULL;
    
    int i = 0; // store index of '>'
    while(i < (index-1) && (strcmp(parsedCommand[i], redirectionSymbol) != 0)) {
        i++;
    }

    if(strcmp(parsedCommand[i], redirectionSymbol) == 0) {
        if (i == 0) { // no command specified
            write(STDERR_FILENO, redirectionError, strlen(redirectionError));
            return;
        }
        else if(i == (index - 1)) { // no file specified
            write(STDERR_FILENO, redirectionError, strlen(redirectionError));
            return;
        } 
        else if (strcmp(parsedCommand[i+1], redirectionSymbol) == 0) { // double redirection symbol
            write(STDERR_FILENO, redirectionError, strlen(redirectionError));
            return;
        }
        else if ((index - i) != 2) { // multiple file specified
            write(STDERR_FILENO, redirectionError, strlen(redirectionError));
            return;
        }
        else {
            outputFileName = parsedCommand[i+1];
            output = fopen(parsedCommand[i+1], "w");
            if(output == NULL) {
                char errorMessage[512] = "";
                strcpy(errorMessage, outputFileError);
                strcat(errorMessage, parsedCommand[i+1]);
                strcat(errorMessage, ".\n");
                write(STDERR_FILENO, errorMessage, strlen(errorMessage));
                exit(1);
            }
            char *commandCopy[maxCommandLength];
            int j;
            for(j = 0; j < i; j++) {
                commandCopy[j] = parsedCommand[j];
            }
            commandCopy[j] = NULL;
            executable(commandCopy, 1);
        }
    }
    else {
        if(strcmp(parsedCommand[0], "alias") == 0) {

        }    
        else if(strcmp(parsedCommand[0], "unalias") == 0) {

        } 
        else if(strcmp(parsedCommand[0], "exit") == 0) {
            exit(0);
        }
        else { // if passed argument is an executable
            executable(parsedCommand, 0);
        }
    }
}

int main(int argc, char** argv) {
    if(argc == 1) { // interactive mode
        while(1) {
            char command[maxCommandLength];
            write(STDOUT_FILENO, commandPrompt, strlen(commandPrompt));
            fgets(command, maxCommandLength, stdin);
            if(feof(stdin) != 0) {
                break;
            }
            parse_Command(command);
        }
    } 

    else if(argc == 2) { // batch mode
        size_t commandLength = maxCommandLength;
        size_t inputLength = 0;
        char *batchCommand = NULL;

        batchFile = fopen(argv[1], "r");
        if(batchFile == NULL) {
            char errorMessage[512] = "";
            strcpy(errorMessage, fileOpenError);
            strcat(errorMessage, argv[1]);
            strcat(errorMessage, ".\n");
            write(STDERR_FILENO, errorMessage, strlen(errorMessage));
            exit(1);
        }
        while((inputLength = getline(&batchCommand, &commandLength, batchFile)) != -1) {
            if(inputLength > maxCommandLength) {
                write(STDOUT_FILENO, batchCommand, strlen(batchCommand));
                continue;
            }
            else {
                write(STDOUT_FILENO, batchCommand, strlen(batchCommand));
                parse_Command(batchCommand);
            }
        }
    }
    else { // invalid argument number
        write(STDERR_FILENO, invalidArgumentNumber, strlen(invalidArgumentNumber));
        return 1;
    }
    return 0;
}
