#ifndef C_SHELL_COMMANDLIST_H
#define C_SHELL_COMMANDLIST_H

#include "jobDeclaration.h"

int run_commands(char **params, struct job **jobList, char *last_cd, char *shell_root, struct job *fgProcess,
                 int *errorCode);

#endif //C_SHELL_COMMANDLIST_H
