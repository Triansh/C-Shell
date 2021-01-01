#ifndef C_SHELL_JOBS_H
#define C_SHELL_JOBS_H

#include "jobDeclaration.h"

struct job *createJob(struct job *jb, int id, pid_t pid, char *p_name);

void addJob(pid_t pid, char *p_name, struct job **jobList);

void checkJob(struct job **jobList);

int getId(long id, struct job **jobList);

void print_jobs(struct job **jobList, int *errorCode);

#endif //C_SHELL_JOBS_H
