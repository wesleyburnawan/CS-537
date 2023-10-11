#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("wordle: invalid number of args\n");
        exit(1);
    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("wordle: cannot open file\n");
        exit(1);
    }
    char str[256] = "";
    while(fgets(str, sizeof(str), fp) != NULL) {
        if(strlen(str) == 6) {
            int contains = 0;
            for(int i = 0; i < strlen(argv[2]); i++) {
                if(strchr(str, argv[2][i]) != NULL) {
                    contains++;
                }
            }
            if(contains == 0) {
                printf("%s", str);
            }
        }
    }
    fclose(fp);
    return 0;
}