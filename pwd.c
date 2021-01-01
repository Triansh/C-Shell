#include "headers.h"
#include "pwd.h"

void execute_pwd(int *errorCode) {

    char *dir = malloc(path_len);
    getcwd(dir, path_len);
    printf("%s\n", dir);
    free(dir);
    *errorCode = 0;

}
