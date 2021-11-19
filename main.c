#include "headers.h"
#include "executeCommand.h"
#include "prompt.h"

static sigjmp_buf env;
struct job *jobList[TOTAL_JOBS];
char last_cd_command[path_len];
struct job *fgProcess;
int errorCode = -1;

void SIGINT_handler(int signum) {

    printf("\n");
    siglongjmp(env, 42);
}

void SIGTSTP_handler(int signum) {

    printf("\n");
    siglongjmp(env, 42);
}

void SIGCHLD_handler(int signum) {

    if(signum == SIGINT){
        errorCode = 1;
    }

    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("%d", pid);
        for (int i = 0; i < TOTAL_JOBS; ++i) {
            if (jobList[i] != NULL && jobList[i]->pid == pid) {

                if (WIFEXITED(status)) {
                    printf("%s with pid %d exited normally\n", jobList[i]->command, jobList[i]->pid);
                } else if (WIFSIGNALED(status)) {
                    printf("%s with pid %d exited by receiving a signal\n", jobList[i]->command,
                           jobList[i]->pid);
                } else if (WIFSTOPPED(status)) {
                    printf("%s with pid %d stopped by receiving a signal\n", jobList[i]->command,
                           jobList[i]->pid);
                } else {
                    printf("%s with pid %d exited abnormally\n", jobList[i]->command, jobList[i]->pid);
                }

                free(jobList[i]);
                jobList[i] = NULL;
            }
        }
    }

}


int main(void) {

    char shell_root[path_len];
    memset(shell_root, 0, sizeof(shell_root));
    strcpy(shell_root, getenv("PWD"));

    memset(last_cd_command, 0, sizeof(last_cd_command));
    strcpy(last_cd_command, shell_root);

    fgProcess = malloc(sizeof(*fgProcess));
    fgProcess->id = -1;
    for (int i = 0; i < TOTAL_JOBS; i++) {
        jobList[i] = NULL;
    }

    printf(GREEN "\n*****************************************************\n" RESET);
    printf(YELLOW "                   Hello %s,\n" RESET, getenv("USERNAME"));
    printf(BLUE "             Welcome to Triansh's SHELL" RESET);
    printf(GREEN "\n*****************************************************\n\n" RESET);


    STDIN_COPY = dup(0);
    STDOUT_COPY = dup(1);

    bool run = true;
    while (run) {

        while (sigsetjmp(env, 42) != 0);

        signal(SIGINT, SIGINT_handler);
        signal(SIGCHLD, SIGCHLD_handler);
        signal(SIGTSTP, SIGTSTP_handler);

        char **commands = prompt(shell_root, &errorCode);

        for (int i = 0; commands[i] != NULL; i++) {
            if (execute_commands(commands[i], jobList, last_cd_command, shell_root, fgProcess, &errorCode)) {
                run = false;
            }
        }

        for (int i = 0; i < CF_count; i++) free(commands[i]);
        free(commands);
    }

    free(fgProcess);
    for (int i = 0; i < TOTAL_JOBS; i++) {
        free(jobList[i]);
    }

    return 0;

}
// add more color if possible
// check errorCode in jobs, ls and fg + Ctrl-Z + C