#include "headers.h"
#include "killProcess.h"
#include "jobs.h"

void execute_kjob(char **params, struct job **jobList, int *errorCode) {

    if (params[3] != NULL) {
        printf("kjob: To many arguments.\n");
        *errorCode = 1;
        return;
    } else if (params[1] == NULL || params[2] == NULL) {
        printf("kjob: Please provide both job number and signal number\n");
        *errorCode = 1;
        return;
    }

    char *str = NULL;
    long jid = strtol(params[1], &str, 10);
    if (!(str == NULL || strcmp("", str) == 0)) {
        printf("kjob: Invalid Argument %s - Must be a job id\n", params[1]);
        *errorCode = 1;
        return;
    }

    int index;
    if ((index = getId(jid, jobList)) == -1) {
        printf("kjob: No process with given job id %ld\n", jid);
        *errorCode = 1;
        return;
    }

    int kill_code = (int) strtol(params[2], &str, 10);
    if (!(str == NULL || strcmp("", str) == 0)) {
        printf("kjob: Invalid Argument %s - Must be a signal code\n", params[1]);
        *errorCode = 1;
        return;
    }

    if (kill(jobList[index]->pid, kill_code) < 0) {
        perror("kjob: Unable to kill process");
        *errorCode = 1;
        return;
    }
    *errorCode = 0;

}

void overkill(struct job **jobList, int *errorCode) {

    for (int i = 0; i < TOTAL_JOBS; i++) {
        if (jobList[i] != NULL) {
            if (kill(jobList[i]->pid, SIGKILL) < 0) {
                printf("Error while killing process with pid %d ", jobList[i]->pid);
                perror("");
                fflush(stdout);
            }
//            sleep(1);
        }
    }
    *errorCode = 0;

}

