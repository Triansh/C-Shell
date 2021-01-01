#ifndef C_SHELL_JOBDECLARATION_H
#define C_SHELL_JOBDECLARATION_H

struct job {
    int pid;
    int id;
    char command[100];
    char message[100];
};

#endif
