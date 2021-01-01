#include "headers.h"
#include "envVariables.h"

void setEnv(char **params, int *errorCode) {

    if (params[1] == NULL) {
        printf("setenv: Very few arguments\n");
        *errorCode = 1;
        return;
    } else if (params[3] != NULL) {
        printf("setenv: To many arguments\n");
        *errorCode = 1;
        return;
    }

    char name[strlen(params[1]) + 2];
    memset(name, 0, sizeof(name));
    strcpy(name, params[1]);

    char *value = malloc(100);
    memset(value, 0, sizeof(value));
    if (params[2] != NULL) strcpy(value, params[2]);

    if (setenv(name, value, 1) < 0) {
        perror("setenv: Error in setting");
        *errorCode = 1;
        free(value);
        return;
    }

    free(value);
    *errorCode = 0;

}

void unsetEnv(char **params, int *errorCode) {

    if (params[1] == NULL) {
        printf("unsetenv: Very few arguments\n");
        *errorCode = 1;
        return;
    } else if (params[2] != NULL) {
        printf("unsetenv: To many arguments\n");
        *errorCode = 1;
        return;
    }

    if (getenv(params[1]) == NULL) {
        printf("%s is not an environment variable\n", params[1]);
        return;
    }

    if (unsetenv(params[1]) < 0) {
        perror("unsetenv: Error in unsetting");
        *errorCode = 1;
        return;
    }
    *errorCode = 0;


}