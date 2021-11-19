#include "headers.h"
#include "changeProcessGroup.h"
#include "jobs.h"

void perform_fg(char **params, struct job **jobList, struct job *fgProcess, int *errorCode) {

    if (params[2] != NULL) {
        printf("fg: To many arguments.\n");
        *errorCode = 1;
        return;
    } else if (params[1] == NULL) {
        printf("fg: Please provide a job number\n");
        *errorCode = 1;
        return;
    }

    char *str = NULL;
    long jid = strtol(params[1], &str, 10);
    if (!(str == NULL || strcmp("", str) == 0)) {
        printf("fg: Invalid Argument %s - Must be a job id\n", params[1]);
        *errorCode = 1;
        return;
    }

    int index;
    if ((index = getId(jid, jobList)) == -1) {
        printf("fg: No process with given job id %ld\n", jid);
        *errorCode = 1;
        return;
    }

    if (kill(-jobList[index]->pid, SIGCONT) < 0) {
        perror("fg: job not found\n");
        *errorCode = 1;
        return;
    }

    pid_t job_pid = jobList[index]->pid;
    fgProcess = createJob(fgProcess, 1, job_pid, jobList[index]->command);
    free(jobList[index]);
    jobList[index] = NULL;

    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    if (tcsetpgrp(0, getpgid(job_pid)) < 0) {
        perror("fg: Unable to bring process in foreground");
        *errorCode = 1;
        return;
    }

    printf("jkbdkbbw\n");

    int status;
    waitpid(-(job_pid), &status, WUNTRACED);

    printf("jkbdkbbw\n");


    *errorCode = 0;

    if (tcsetpgrp(0, getpgid(getpid())) < 0) {
        perror("fg: Unable to bring process in foreground");
        *errorCode = 1;
        return;
    }

    printf("jkbdkbbw\n");
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);

    printf("status: %d\n", status);

    if (WIFSTOPPED(status)) {
        printf("\nStopped ");
        addJob(fgProcess->pid, fgProcess->command, jobList);
        *errorCode = 1;
    } else if (WIFEXITED(status)) {
		printf("[%d] ", fgProcess->pid);
		printf("done");
		printf("\t %s\n", fgProcess->command);
		// delete(j->_pgid, jobList);
		// fflush(stdout);
	} else if (WIFSIGNALED(status)) {
		printf("[%d] ", fgProcess->pid);
		printf(" with signal %d", WTERMSIG(status));
		printf("\t %s\n", fgProcess->command);
		// fflush(stdout);			
	}
    fgProcess->id = -1;
    printf("jkbdkbbw\n");

}

void perform_bg(char **params, struct job **jobList, int *errorCode) {

    if (params[2] != NULL) {
        printf("bg: To many arguments\n");
        *errorCode = 1;
        return;
    } else if (params[1] == NULL) {
        printf("bg: Please provide a job number\n");
        *errorCode = 1;
        return;
    }

    char *str = NULL;
    long jid = strtol(params[1], &str, 10);
    if (!(str == NULL || strcmp("", str) == 0)) {
        printf("bg: Invalid Argument %s - Must be a job id\n", params[1]);
        *errorCode = 1;
        return;
    }

    int index;
    if ((index = getId(jid, jobList)) == -1) {
        printf("bg: No process with given job id %ld\n", jid);
        *errorCode = 1;
        return;
    }

    if (kill(jobList[index]->pid, SIGCONT) < 0) {
        printf("fg: job not found\n");
        *errorCode = 1;
        return;
    }
}
// issues - joblist is not cleared for back process exit, same with Ctrl C and Ctrl Z