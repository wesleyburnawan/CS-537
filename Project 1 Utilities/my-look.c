#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

void toAlphaNum(char *str) {
    int i = 0;
    while(str[i] != '\n') {
        if(isalnum(str[i])) {
            i++;
        }
        else {
            int j = i;
            while(str[j] != '\n') {
                str[j] = str[j+1];
                j++;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int opt;
    opterr = 0;
    while((opt = getopt(argc, argv, "Vhf:")) != 1) {
        switch(opt) {
            case 'V':
                printf("my-look from CS537 Spring 2022\n");
                exit(0);
            case 'h':
                printf("Use -V command to display information about this utility\n");
                printf("Use -h command to display help information about this utility\n");
                printf("Use -f command followed by a filename and a string to display all the words in the filename with a prefix of the string\n");
                exit(0);
            case 'f':
                if(strcmp(optarg, argv[argc-1]) != 0) {
                    FILE *fp = fopen(optarg, "r");
                    if (fp == NULL) {
                        printf("my-look: cannot open file\n");
                        exit(1);
                    }
                    char str[256] = "";
                    while(fgets(str, sizeof(str), fp) != NULL) {
                        char unchanged[256] = "";
                        strcpy(unchanged, str);
                        toAlphaNum(str);
                        if(strncasecmp(str, argv[argc-1], strlen(argv[argc-1])) == 0) {
                            printf("%s", unchanged);
                        }
                    }
                    fclose(fp);
                    exit(0);
                }
            case ':':
            case '?':
                printf("my-look: invalid command line\n");
                exit(1);
        }
        char input[256] = "";
        while(fgets(input, sizeof(input), stdin) != NULL) {
            char unchanged[256] = "";
            strcpy(unchanged, input);
            toAlphaNum(input);
            if(strncasecmp(input, argv[optind], strlen(argv[optind])) == 0) {
                printf("%s", unchanged);
            }
        }
        exit(0);
    }
    return 0;
}