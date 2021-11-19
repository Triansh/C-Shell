#include "headers.h"
#include "stringTokenize.h"
#include "history.h"
#include "prompt.h"


char **prompt(char *shell_root, int *errorCode) {

    char hostname[path_len];
    char username[path_len];

    memset(hostname, 0, sizeof(hostname));
    memset(username, 0, sizeof(username));

    strcpy(username, getenv("USER"));
    gethostname(hostname, path_len);

    char *inputBuffer = (char *) malloc(path_len);
    char homeDir[path_len];
    char cwd[path_len];
    size_t bf_size = 1024;

    memset(cwd, 0, sizeof(cwd));
    memset(homeDir, 0, sizeof(homeDir));

    getcwd(cwd, path_len);

    bool tilda = true;
    unsigned int len = strlen(shell_root);
    if (strlen(cwd) >= len && strncmp(shell_root, cwd, len) == 0) {
        strcpy(homeDir, cwd + len);
    } else {
        strcpy(homeDir, cwd);
        tilda = false;
    }

    if (*errorCode == 1) printf(RED ":'( " RESET);
    else if (*errorCode == 0) printf(GREEN ":') " RESET);

    printf(MAGENTA "%s@%s" RESET, username, hostname);
    printf(":");

    tilda ? printf(CYAN "~%s> "  RESET, homeDir) : printf(CYAN "%s> " RESET, homeDir);

    *errorCode = 0;

    if (getline(&inputBuffer, &bf_size, stdin) == -1) {
        printf("\n");
        free(inputBuffer);
        exit(0);
    }

    char **commands = parseString(inputBuffer, CMD_DELIMITER);

    if (commands[0] != NULL) {
        add_to_history(inputBuffer, shell_root);
    }

    free(inputBuffer);

    return commands;
}