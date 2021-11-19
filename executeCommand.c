#include "headers.h"
#include "executeCommand.h"
#include "stringTokenize.h"
#include "commandList.h"

// ls will give -l output wth ls < a.txt -l
int *check_redirection(char **args, int *errorCode) {

    char *infile = NULL;
    char *outfile = NULL;
    bool append = false;
    int in = -1, out = -1;

    int *fd = malloc(sizeof(int) * 2);
    fd[0] = fd[1];

    for (int i = 0; args[i] != NULL; i++) {

        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            if (args[i + 1] == NULL) {
                *errorCode = 1;
                dup2(STDOUT_COPY, 1);
                printf("Invalid Syntax\n");
                fd[1] = -2;
                return fd;
            } else {
                outfile = malloc(100);
                out = i;
                append = (strcmp(args[i], ">>") == 0);
                strcpy(outfile, args[i + 1]);
            }
        } else if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] == NULL) {
                *errorCode = 1;
                dup2(STDOUT_COPY, 1);
                printf("Invalid Syntax\n");
                fd[0] = -2;
                return fd;
            } else {
                infile = malloc(100);
                in = i;
                strcpy(infile, args[i + 1]);
            }
        }
    }

    if (in != -1) {
        free(args[in]);
        free(args[in + 1]);
        args[in] = NULL;
        args[in + 1] = NULL;
        fd[0] = open(infile, O_RDONLY);
        if (fd[0] < 0) {
            *errorCode = 1;
            fd[0] = -2;
            perror("Unable to access file");
            return fd;
        }
    }
    if (out != -1) {
        free(args[out]);
        free(args[out + 1]);
        args[out] = NULL;
        args[out + 1] = NULL;
        fd[1] = !append ? open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644)
                        : open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd[1] < 0) {
            fd[1] = -2;
            *errorCode = 1;
            perror("Unable to access file");
            return fd;
        }
    }

    free(infile);
    free(outfile);


    return fd;
}

int execute(char *cmd, struct job **jobList, char *last_cd, char *shell_root, struct job *fgProcess, int *errorCode) {
    char **args = parseString(cmd, ARG_DELIMITER);
    int *fds = check_redirection(args, errorCode);

    if (fds[0] == -2 || fds[1] == -2) return 0;

    if (fds[0] >= 0) dup2(fds[0], 0);
    if (fds[1] >= 0) dup2(fds[1], 1);

//    for (int j = fds[2]; fds[2] >= 0 && args[j] != NULL; j++) {
//        free(args[j]);
//        args[j] = NULL;
//    }

    int ret_val = run_commands(args, jobList, last_cd, shell_root, fgProcess, errorCode);

    for (int i = 0; i < CF_count; ++i) {
        free(args[i]);
    }
    free(args);
    free(fds);
    return ret_val;
}

// error handling
int go_through_piping(char *inputbuffer, struct job **jobList, char *last_cd, char *shell_root, struct job *fgProcess,
                      int *errorCode) {

    int total_pipes = 0, len = strlen(inputbuffer);
    for (int i = 0; i < len; ++i) {
        if (inputbuffer[i] == '|') total_pipes++;
    }

    char **pipe_div = parseString(inputbuffer, "|");

    int argc = getArgumentCount(pipe_div);

    if (argc != total_pipes + 1) {
        *errorCode = 1;
        printf("Invalid Syntax\n");
        for (int i = 0; i < CF_count; ++i) {
            free(pipe_div[i]);
        }
        free(pipe_div);
        return 0;
    }

    int ret_val = 0;

    if(argc > 1){

        int pipe_fd[2];
        int in_fd = STDIN_COPY;

        for (int i = 0; i < argc ; i++) {
            if (pipe(pipe_fd) < 0) {
                perror("Error in constructing pipe");
                break;
            }

            pid_t childpid = fork();

            if(childpid == -1){
                perror("Error in creating child");
                break;
            } else if (childpid == 0){
                dup2(in_fd, 0);

                i != argc - 1 ? dup2(pipe_fd[1], 1) : dup2(STDOUT_COPY, 1);
                close(pipe_fd[0]);
                execute(pipe_div[i], jobList, last_cd, shell_root, fgProcess, errorCode);
                exit(0);

            } else{
                wait(NULL);
                in_fd = pipe_fd[0];
                close(pipe_fd[1]);
            }

        }
    } else if (execute(pipe_div[0], jobList, last_cd, shell_root, fgProcess, errorCode)) {
        ret_val = 1;
    }


    for (int i = 0; i < CF_count; ++i) {
        free(pipe_div[i]);
    }
    free(pipe_div);

    return ret_val;

}

int execute_commands(char *cmd, struct job **jobList, char *last_cd, char *shell_root, struct job *fgProcess,
                     int *errorCode) {
    int ret_val = go_through_piping(cmd, jobList, last_cd, shell_root, fgProcess, errorCode);

    dup2(STDIN_COPY, 0);
    dup2(STDOUT_COPY, 1);

    return ret_val;
}

// problem with sort & , checking grep