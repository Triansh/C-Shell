
#ifndef C_SHELL_CHANGEPROCESSGROUP_H
#define C_SHELL_CHANGEPROCESSGROUP_H
#include "jobDeclaration.h"

void perform_fg(char **params, struct job **jobList, struct job *fgProcess, int *errorCode);

void perform_bg(char **params, struct job **jobList, int *errorCode);

#endif //C_SHELL_CHANGEPROCESSGROUP_H
