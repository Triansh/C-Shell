#include "headers.h"
#include "ls.h"

char **extract_flags(char **params) {

    char **flags = malloc(CF_count * sizeof(char *));
    for (int i = 0; i < CF_count; i++) flags[i] = NULL;

    for (int i = 0, k = 0; i < CF_count; ++i) {
        if (params[i] != NULL && params[i][0] == '-') {
            flags[k] = malloc(path_len);
            strcpy(flags[k], params[i] + 1);
            k++;
        }
    }

    return flags;
}

char **extract_args(char **params) {

    char **args = malloc(CF_count * sizeof(char *));
    for (int i = 0; i < CF_count; i++) args[i] = NULL;

    for (int i = 1, k = 0; params[i] != NULL; ++i) {
        if (params[i][0] != '-') {
            args[k] = malloc(path_len);
            strcpy(args[k], params[i]);
            k++;
        }
    }

    return args;
}

void print_details(struct stat file_stats, int year, char *file_name) {

    if (S_ISLNK(file_stats.st_mode)) printf("l");
    else if (S_ISREG(file_stats.st_mode)) printf("-");
    else if (S_ISDIR(file_stats.st_mode)) printf("d");
    else if (S_ISCHR(file_stats.st_mode)) printf("c");
    else if (S_ISBLK(file_stats.st_mode)) printf("b");
    else if (S_ISFIFO(file_stats.st_mode)) printf("p");
    else if (S_ISSOCK(file_stats.st_mode)) printf("s");

    printf("%c", ((file_stats.st_mode & S_IRUSR) ? 'r' : '-'));
    printf("%c", ((file_stats.st_mode & S_IWUSR) ? 'w' : '-'));
    printf("%c", ((file_stats.st_mode & S_IXUSR) ? 'x' : '-'));
    printf("%c", ((file_stats.st_mode & S_IRGRP) ? 'r' : '-'));
    printf("%c", ((file_stats.st_mode & S_IWGRP) ? 'w' : '-'));
    printf("%c", ((file_stats.st_mode & S_IXGRP) ? 'x' : '-'));
    printf("%c", ((file_stats.st_mode & S_IROTH) ? 'r' : '-'));
    printf("%c", ((file_stats.st_mode & S_IWOTH) ? 'w' : '-'));
    printf("%c", ((file_stats.st_mode & S_IXOTH) ? 'x' : '-'));

    printf("\t%ld\t", (long) file_stats.st_nlink);

    struct passwd *user;
    struct group *grp;
    user = getpwuid(file_stats.st_uid);
    grp = getgrgid(file_stats.st_gid);

    printf("%s\t", user->pw_name);
    printf("%s\t", grp->gr_name);

    printf("%lld\t", (long long) file_stats.st_size);

    struct tm *new_time;
    new_time = localtime(&file_stats.st_mtime);

    char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    printf("%s\t", months[new_time->tm_mon]);
    printf("%d\t", new_time->tm_mday);

    if (year == (int) new_time->tm_year + 1900) {
        new_time->tm_hour >= 10 ? printf("%d:", new_time->tm_hour) : printf("0%d:", new_time->tm_hour);
        new_time->tm_min >= 10 ? printf("%d\t", new_time->tm_min) : printf("0%d\t", new_time->tm_min);
    } else {
        printf("%d\t", new_time->tm_year + 1900);
    }

    printf("%s\t", file_name);
    printf("\n");


}


void print_for_directory(char *path, struct dirent *file, bool a, bool l, int year, int *errorCode) {

    if (l) {

        if (file->d_name[0] != '.' || (a)) {

            char *new_file = malloc(path_len);
            strcpy(new_file, path);

            unsigned int len = strlen(new_file);
            if (new_file[len - 1] != '/') {
                strcat(new_file, "/");
            }

            strcat(new_file, file->d_name);

            struct stat file_stats;
            if (stat(new_file, &file_stats) < 0) {
                perror("Cannot Access file");
                free(new_file);
                *errorCode = 1;
                return;
            }

            print_details(file_stats, year, file->d_name);
            *errorCode = 0;

            free(new_file);

        }
    } else {

        if (file->d_name[0] != '.' || (a)) {
            printf("%s\n", file->d_name);
        }
    }
}

void execute_ls(char **params, char *shell_root, int *errorCode) {

    char **flags = extract_flags(params);
    char **args = extract_args(params);

    if (args[0] == NULL) {
        args[0] = malloc(path_len);
        getcwd(args[0], path_len);
    }

    bool a_option = false, l_option = false;
    for (int i = 0; flags[i] != NULL; i++) {
        unsigned int len = strlen(flags[i]);
        for (int j = 0; j < len; j++) {
            if (flags[i][j] == 'a') a_option = true;
            if (flags[i][j] == 'l') l_option = true;
        }
    }

    int argc = 0;
    while (args[argc] != NULL) argc++;

    struct tm *curr_time;
    time_t ts = time(0);
    curr_time = localtime(&ts);
    int year = curr_time->tm_year + 1900;

    for (int i = 0; args[i] != NULL; i++) {

        if (args[i][0] == '~') {
            char *temp = malloc(path_len);
            strcpy(temp, shell_root);
            strcat(temp, args[i] + 1);
            strcpy(args[i], temp);
            free(temp);
        }

        if (argc > 1) {
            printf("%s:\n", args[i]);
        }


        struct stat file_stat;
        if (stat(args[i], &file_stat) < 0) {
            perror("Cannot access");
            *errorCode = 1;
            continue;
        }

        if (S_ISDIR(file_stat.st_mode)) {

            struct dirent **file_list;

            int no_of_files = scandir(args[i], &file_list, NULL, alphasort);
            if (no_of_files < 0) {
                printf("ls: Cannot access '%s':", args[i]);
                fflush(stdout);
                perror("");
                *errorCode = 1;
                continue;
            }

            long long blocks = 0;

            for (int j = 0; l_option && j < no_of_files; j++) {
                if (file_list[j]->d_name[0] != '.' || (a_option)) {

                    char *file_path = malloc(path_len);
                    strcpy(file_path, args[i]);
                    unsigned int len = strlen(file_path);
                    if (file_path[len - 1] != '/') {
                        strcat(file_path, "/");
                    }
                    strcat(file_path, file_list[j]->d_name);

                    struct stat file;
                    stat(file_path, &file);
                    blocks += file.st_blocks;
                    free(file_path);
                }
            }

            if (l_option) printf("total %lld\n", blocks / 2);

            for (int j = 0; j < no_of_files; j++) {
                print_for_directory(args[i], file_list[j], a_option, l_option, year, errorCode);
            }

            for (int j = 0; j < no_of_files; j++) free(file_list[j]);
            free(file_list);

        } else {
            if (l_option) print_details(file_stat, year, args[i]);
            else printf("%s\n", args[i]);
        }
        *errorCode = 0;

    }

    for (int i = 0; i < CF_count; i++) free(args[i]);
    for (int i = 0; i < CF_count; i++) free(flags[i]);
    free(args);
    free(flags);
}

// while doing not -l we should print multiple dir in single line