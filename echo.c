#include "headers.h"
#include "echo.h"

void execute_echo(char **params, int *errorCode) {

    for (int i = 1; params[i] != NULL; i++) {
        printf("%s ", params[i]);
    }
    printf("\n");
    *errorCode = 0;
}