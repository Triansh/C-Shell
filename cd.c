#include "headers.h"
#include "cd.h"

void execute_cd(char **path, char *last_cd_command, char *shell_root, int *errorCode) {


    if (path[2] != NULL) {
        printf("cd: To many arguments\n");
        *errorCode = 1;
        return;
    }

    if (path[1] == NULL) {
        path[1] = malloc(10);
        strcpy(path[1], "~");
    }

    char *temp = malloc(path_len);
    strcpy(temp, path[1]);
    if (temp[0] == '~') {
        strcpy(temp, shell_root);
        strcat(temp, path[1] + 1);
    } else if (strcmp(path[1], "-") == 0) {
        strcpy(temp, last_cd_command);
        printf("%s\n", last_cd_command);
    } else {
        strcpy(temp, path[1]);
    }

    getcwd(last_cd_command, path_len);


    if (chdir(temp) == -1) {
        free(temp);
        perror("cd: Unable to reach file");
        *errorCode = 1;
        return;
    }
    free(temp);
    *errorCode = 0;

}
