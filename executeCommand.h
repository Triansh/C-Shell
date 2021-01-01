
#ifndef C_SHELL_EXECUTECOMMAND_H
#define C_SHELL_EXECUTECOMMAND_H

#include "jobDeclaration.h"

int execute_commands(char *cmd, struct job **jobList, char *last_cd, char *shell_root, struct job *fgProcess,
                     int *errorCode);

#endif //C_SHELL_EXECUTECOMMAND_H
