#include "headers.h"
#include "pinfo.h"

void execute_pinfo(char **params, char *shell_root, int *errorCode) {

    pid_t pid;
    if (params[1] == NULL) {
        pid = getpid();
    } else if (params[2] != NULL) {
        printf("pinfo: To many arguments\n");
        *errorCode = 1;
        return;
    } else {
        char *st = NULL;
        pid = strtol(params[1], &st, 10);
        if (!(st == NULL || strcmp("", st) == 0)) {
            printf("pinfo: Invalid Argument %s - Must be a number\n", params[1]);
            *errorCode = 1;
            return;
        }
    }

    char p_state[2], vm_size[20];
    char executable_path[path_len];
    char comp_path[path_len];
    char path[50];
    char proc_ex[50];
    char proc_stat[50];

    memset(executable_path, 0, sizeof(executable_path));
    memset(comp_path, 0, sizeof(comp_path));
    memset(path, 0, sizeof(path));
    memset(proc_ex, 0, sizeof(proc_ex));
    memset(proc_stat, 0, sizeof(proc_stat));

    sprintf(path, "/proc/%d/", pid);
    strcpy(proc_ex, path);
    strcat(proc_ex, "exe");
    strcpy(proc_stat, path);
    strcat(proc_stat, "stat");

    FILE *fp = fopen(proc_stat, "r");
    if (fp == NULL) {
        perror("pinfo: Error gathering information about process");
        *errorCode = 1;
        return;
    }

    int pgrp, tpgid;

    for (int i = 1; i <= 23; i++) {
        if (i == 3) {
            fscanf(fp, "%s", p_state);
        } else if (i == 5) {
            fscanf(fp, "%d", &pgrp);
        } else if (i == 8) {
            fscanf(fp, "%d", &tpgid);
        } else if (i == 23) {
            fscanf(fp, "%s", vm_size);
        } else {
            fscanf(fp, "%*s");
        }
    }

    fclose(fp);

    if (readlink(proc_ex, executable_path, path_len) < 0) {
        perror("pinfo: Error gathering information about process");
        *errorCode = 1;
        return;
    }

    bool tilda = true;
    unsigned int len = strlen(shell_root);
    if (strlen(executable_path) >= len && strncmp(shell_root, executable_path, len) == 0) {
        strcpy(comp_path, executable_path + len);
    } else {
        strcpy(comp_path, executable_path);
        tilda = false;
    }

    printf("pid -- %d\n", pid);
    printf("Process State -- %s", p_state);
    pgrp == tpgid ? printf("+\n") : printf("\n");
    printf("memory -- %s\n", vm_size);
    printf("Executable Path -- ");
    tilda ? printf("~%s\n", comp_path) : printf("%s\n", comp_path);
    *errorCode = 0;

}