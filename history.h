#ifndef C_SHELL_HISTORY_H
#define C_SHELL_HISTORY_H

void get_history(char **params, char *shell_root, int *errorCode);

void add_to_history(char *input, char *shell_root);

#endif //C_SHELL_HISTORY_H
