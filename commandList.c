#include "headers.h"
#include "commandList.h"
#include "cd.h"
#include "pwd.h"
#include "echo.h"
#include "envVariables.h"
#include "ls.h"
#include "pinfo.h"
#include "history.h"
#include "otherCommands.h"
#include "changeProcessGroup.h"
#include "jobs.h"
#include "killProcess.h"


int run_commands(char **params, struct job **jobList, char *last_cd, char *shell_root, struct job *fgProcess,
                 int *errorCode) { // command list

    if (params[0] == NULL) {
        return 0;
    }

    if (strcmp("quit", params[0]) == 0) {
        return 1;
    }

    if (strcmp("cd", params[0]) == 0) {
        execute_cd(params, last_cd, shell_root, errorCode);
    } else if (strcmp("pwd", params[0]) == 0) {
        execute_pwd(errorCode);
    } else if (strcmp("echo", params[0]) == 0) {
        execute_echo(params, errorCode);
    } else if (strcmp("setenv", params[0]) == 0) {
        setEnv(params, errorCode);
    } else if (strcmp("unsetenv", params[0]) == 0) {
        unsetEnv(params, errorCode);
    } else if (strcmp("ls", params[0]) == 0) {
        execute_ls(params, shell_root, errorCode);
    } else if (strcmp("pinfo", params[0]) == 0) {
        execute_pinfo(params, shell_root, errorCode);
    } else if (strcmp("history", params[0]) == 0) {
        get_history(params, shell_root, errorCode);
    } else if (strcmp("jobs", params[0]) == 0) {
        print_jobs(jobList, errorCode);
    } else if (strcmp("fg", params[0]) == 0) {
        perform_fg(params, jobList, fgProcess, errorCode);
    } else if (strcmp("bg", params[0]) == 0) {
        perform_bg(params, jobList, errorCode);
    } else if (strcmp("kjob", params[0]) == 0) {
        execute_kjob(params, jobList, errorCode);
    } else if (strcmp("overkill", params[0]) == 0) {
        overkill(jobList, errorCode);
    } else {
        run_others(params, jobList, fgProcess, errorCode);
    }

    return 0;

}
