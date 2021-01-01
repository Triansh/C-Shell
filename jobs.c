#include "headers.h"
#include "jobs.h"

int compare(const void *a, const void *b) {
    const struct job *arg1 = *(const struct job **) a;
    const struct job *arg2 = *(const struct job **) b;
    if (!arg1 && !arg2) return 0;
    if (!arg1) return 1;
    if (!arg2) return -1;
    if (arg1->id < arg2->id) return -1;
    if (arg1->id > arg2->id) return 1;
    return 0;
}

struct job *createJob(struct job *jb, int id, pid_t pid, char *p_name) {

//    jb = malloc(sizeof(*jb));
    jb->id = id;
    jb->pid = pid;
    strcpy(jb->command, p_name);
    memset(jb->message, 0, sizeof(jb->message));
    return jb;
}

int getNewId(struct job **jobList) {

    int newId = 0;
    for (int i = 0; i < TOTAL_JOBS; i++) {
        if (jobList[i] != NULL) {
            newId = jobList[i]->id > newId ? jobList[i]->id : newId;
        }
    }
    return newId + 1;
}

void addJob(pid_t pid, char *p_name, struct job **jobList) {

    int newId = getNewId(jobList);
    printf("[%d] \t %d\n", newId, pid);

    for (int i = 0; i < TOTAL_JOBS; i++) {
        if (jobList[i] == NULL) {
            jobList[i] = malloc(sizeof(*jobList[i]));
            jobList[i] = createJob(jobList[i], newId, pid, p_name);
            break;
        }
    }

    qsort(jobList, TOTAL_JOBS, sizeof(struct job *), compare);

}

void print_jobs(struct job **jobList, int *errorCode) {

    qsort(jobList, TOTAL_JOBS, sizeof(struct job *), compare);

    char *buffer = malloc(100);

    for (int i = 0; jobList[i] != NULL; i++) {

        if (strcmp("", jobList[i]->message) == 0) {

            sprintf(buffer, "/proc/%d/stat", jobList[i]->pid);
            FILE *fp = fopen(buffer, "r");
            if (fp == NULL) {
                perror("jobs: Error gathering information about process");
                continue;
            }

            char p_state[2];
            fscanf(fp, "%*s %*s %s", p_state);
            fclose(fp);
            printf("[%d] %s %s [%d]\n", jobList[i]->id, p_state[0] == 'T' ? "Stopped" : "Running",
                   jobList[i]->command, jobList[i]->pid);
        }
    }
    free(buffer);
    *errorCode = 0;

}


int getId(long id, struct job **jobList) {

    for (int i = 0; i < TOTAL_JOBS; i++) {
        if (jobList[i] != NULL && jobList[i]->id == id) {
            return i;
        }
    }
    return -1;
}