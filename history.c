#include "headers.h"
#include "stringTokenize.h"
#include "history.h"

void get_history(char **params, char *shell_root, int *errorCode) {

    long line_def;

    if (params[1] == NULL) {
        line_def = 10;
    } else if (params[2] != NULL) {
        printf("history: To many arguments\n");
        *errorCode = 1;
        return;
    } else {

        char *str = NULL;
        char *copy = malloc(path_len);
        strcpy(copy, params[1]);

        line_def = strtol(copy, &str, 10);
        if (!(str == NULL || strcmp("", str) == 0)) {
            printf("history: Invalid Argument %s - Must be an integer\n", copy);
            free(copy);
            *errorCode = 1;
            return;
        }
        free(copy);
    }

    char *file_path = malloc(path_len);
    strcpy(file_path, shell_root);
    strcat(file_path, "/history.txt");

    FILE *fp = fopen(file_path, "r");

    free(file_path);

    if (fp == NULL) {
        perror("history: Unable to open file");
        *errorCode = 1;
        return;
    }

    char **history = malloc(sizeof(char *) * 21);
    for (int i = 0; i < 21; i++) history[i] = NULL;

    int lines_got = 0;
    size_t line_len = 0;
    while (getline(&history[lines_got], &line_len, fp) != -1) {
        lines_got++;
    }

    if (fclose(fp) != 0) {
        perror("history: Unable to close file");
    }

    if (line_def > lines_got) line_def = lines_got;

    for (long i = lines_got - line_def; i < lines_got; i++) {
        printf("%s", history[i]);
    }

    for (int i = 0; i < 21; i++) free(history[i]);
    free(history);
    *errorCode = 0;

}

bool check_last_command(char *last_input, char *curr_input) {

    bool eq_cmd = true;

    char **curr_com = parseString(curr_input, ARG_DELIMITER);
    char **last_com = parseString(last_input, ARG_DELIMITER);

    int tot = 0;
    while (last_com[tot] != NULL) tot++;

    for (int i = 0; i < tot; i++) {
        if (curr_com[i] == NULL || (strcmp(last_com[i], curr_com[i]) != 0)) {
            eq_cmd = false;
            break;
        }
    }

    if (eq_cmd && curr_com[tot] != NULL) eq_cmd = false;

    for (int i = 0; i < CF_count; i++) {
        free(curr_com[i]);
        free(last_com[i]);
    }
    free(curr_com);
    free(last_com);

    return eq_cmd;
}


void add_to_history(char *input, char *shell_root) {

    char *file_path = malloc(path_len);
    strcpy(file_path, shell_root);
    strcat(file_path, "/history.txt");

    FILE *fp = fopen(file_path, "a+");

    free(file_path);

    if (fp == NULL) {
        perror("history: Unable to open file");
        return;
    }

    size_t line_len = 0;
    char **history = malloc(sizeof(char *) * 21);
    for (int i = 0; i < 21; i++) history[i] = NULL;

    int lines_got = 0;
    while (getline(&history[lines_got], &line_len, fp) != -1) {
        lines_got++;
    }

    bool eq_cmd = lines_got <= 0 ? false : check_last_command(history[lines_got - 1], input);

    if (!eq_cmd) {

        ftruncate(fileno(fp), 0);
        for (int i = (lines_got >= 20 ? 1 : 0); i < lines_got; i++) {
            fprintf(fp, "%s", history[i]);
        }
        fprintf(fp, "%s", input);
    }

    if (fclose(fp) != 0) {
        perror("history: Unable to close file");
    }
    for (int i = 0; i < 21; i++) free(history[i]);
    free(history);

}