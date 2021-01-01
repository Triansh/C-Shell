#include "headers.h"
#include "stringTokenize.h"

char **parseString(char *s, char *delimiters) {

    char **argArray = malloc(CF_count * sizeof(char *));
    for (int i = 0; i < CF_count; i++) argArray[i] = NULL;


    if (s != NULL) {
        char *s_copy = malloc(path_len);
        strcpy(s_copy, s);

        char *token = strtok(s_copy, delimiters);
        for (int i = 0; token != NULL; ++i) {
            argArray[i] = malloc(strlen(token) + 2);
            strcpy(argArray[i], token);
            token = strtok(NULL, delimiters);
        }
        free(token);
        free(s_copy);
    }


    return argArray;
}

int getArgumentCount(char **params) {

    int tot = 0;
    for (int i = 0; i < CF_count; i++) {
        if (params[i] != NULL) {

            char **div = parseString(params[i], ARG_DELIMITER);
            if (div[0] != NULL) tot++;
            for (int j = 0; j < CF_count; j++) {
                free(div[j]);
            }
            free(div);

        }
    }
    return tot;

}