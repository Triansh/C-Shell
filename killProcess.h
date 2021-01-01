//
// Created by triansh on 18/09/20.
//

#ifndef C_SHELL_KILLPROCESS_H
#define C_SHELL_KILLPROCESS_H
#include "jobDeclaration.h"

void overkill(struct job **jobList, int *errorCode);

void execute_kjob(char **params, struct job **jobList, int *errorCode);

#endif //C_SHELL_KILLPROCESS_H
