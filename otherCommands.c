#include "headers.h"
#include "otherCommands.h"
#include "jobs.h"


void run_foreground(char **params, struct job **jobList, struct job *fgProcess, int *errorCode) {

    pid_t pid = fork();

    if (pid < 0) {
        perror("There was an error creating the process");
        *errorCode = 1;
        return;
    } else if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
        setpgid(0, 0);

        if (execvp(params[0], params) < 0) {
            perror("There was an error while executing the process");
            exit(1);
        }
    } else {
        *errorCode = 0;
        dup2(STDIN_COPY, 0);
        dup2(STDOUT_COPY, 1);
        fgProcess = createJob(fgProcess, 1, pid, params[0]);
//        int status;
//        waitpid(pid, &status, 0);

        pid_t childPgid = pid, shell_pid = getpgid(getpid());
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);

        if(tcsetpgrp(0, childPgid) < 0) perror("");
        int status;
        waitpid(childPgid, &status, WUNTRACED);
        if(tcsetpgrp(0, shell_pid) < 0) perror("");


        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);

        if (WIFSTOPPED(status)) {
            printf("\nStopped ");
            addJob(fgProcess->pid, fgProcess->command, jobList);
            *errorCode = 1;
        }

        fgProcess->id = -1;
        return;
    }
}

void run_background(char **params, struct job **jobList, int *errorCode) {

    pid_t pid = fork();

    if (pid < 0) {
        perror("There was an error creating the process");
        *errorCode = 1;
        return;
    } else if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        setpgid(0, 0);

        if (execvp(params[0], params) < 0) {
            perror("There was an error while executing the process");
            exit(1);
        }
    } else {
        *errorCode = 0;
        dup2(STDIN_COPY, 0);
        dup2(STDOUT_COPY, 1);
        addJob(pid, params[0], jobList);
    }
}

void run_others(char **params, struct job **jobList, struct job *fgProcess, int *errorCode) {

    bool is_background = false;

    int argc = 0;
    for (int i = 0; i < CF_count; i++, argc++) {
        if (params[i] != NULL && strcmp(params[i], "&") == 0) {
            params[i] = NULL;
            is_background = true;
            break;
        }
    }

    is_background ? run_background(params, jobList, errorCode) : run_foreground(params, jobList, fgProcess, errorCode);
}