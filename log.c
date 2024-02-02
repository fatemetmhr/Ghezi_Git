#include "ghezi.h"

int show_all_logs(int n){
    if(chdir_ghezi())
        return 1;
    FILE *comm = fopen(all_commits, "r");
    if(comm == NULL)
        return 1;
    char *name = malloc(1024);
    for(int i = 0; i < n; i++){
        if(fscanf(comm, "%s \n", name) <= 0)
            break;
        if(!is_commit_silented(name) && print_commit_informations(name))
            return 1;
    }
    fclose(comm);
    return 0;
}

int show_all_branch_commits(char *bname){
    if(chdir_ghezi() || chdir("branch"))
        return 1;
    FILE *f = fopen(bname, "r");
    if(f == NULL)
        return fprintf(stderr, "no such branch exists"), 0;
    char *name = malloc(1024);
    while(fscanf(f, "%s \n", name) > 0)
        if(!is_commit_silented(name) && print_commit_informations(name))
            return 1;
    fclose(f);
    return 0;
}

int show_all_personal_commits(char *pname){
    if(chdir_ghezi())
        return 1;
    FILE *f = fopen(all_commits, "r");
    char *name = malloc(1024);
    while(fscanf(f, "%s \n", name) > 0){
        FILE *infor = fopen(string_concat(get_commit_path(name), "/", commit_user), "r");
        char s[1024];
        fscanf(infor, "%s", s);
        fclose(infor);
        if(!strcmp(s, pname) && !is_commit_silented(name) && print_commit_informations(name))
            return 1;
    }
    fclose(f);
    return 0;
}

int show_all_word_match_commits(int n, char **pat){
    if(chdir_ghezi())
        return 1;
    FILE *comm = fopen(all_commits, "r");
    if(comm == NULL)
        return 1;
    char *name = malloc(1024);
    while(fscanf(comm, "%s \n", name) > 0){
        FILE *msg = fopen(string_concat(get_commit_path(name), "/", commit_message), "r");
        bool re = false;
        char av[MAX_COMMIT_MESSAGE_SIZE + 100];
        fgets(av, sizeof(av), msg);
        fclose(msg);
        for(int i = 0; i < n; i++){
            char *ptr = strstr(av, pat[i]);
            int lenpat = strlen(pat[i]);
            while(ptr != NULL){
                re |= (ptr == av || is_white_space(*(ptr - 1))) && is_white_space(*(ptr + lenpat));
                ptr = strstr(ptr + 1, pat[i]);
            }
        }
        if(re && !is_commit_silented(name) && print_commit_informations(name))
            return 1;
    }
    fclose(comm);
    return 0;
}

int show_all_during_commits(struct tm frm, struct tm bfr){
    if(chdir_ghezi())
        return 1;
    FILE *comm = fopen(all_commits, "r");
    if(comm == NULL)
        return 1;
    char *name = malloc(1024);
    while(fscanf(comm, "%s \n", name) > 0){
        FILE *tim = fopen(string_concat(get_commit_path(name), "/", commit_time), "r");
        if(tim == NULL)
            return 1;
        struct tm cur;
        fscanf(tim, "%d %d %d %d %d %d", &cur.tm_year, &cur.tm_mon, &cur.tm_mday, &cur.tm_hour, &cur.tm_min, &cur.tm_sec);
        fclose(tim);
        if(is_less_equal(frm, cur) && is_less_equal(cur, bfr) && !is_commit_silented(name) && print_commit_informations(name))
            return 1;
    }
    fclose(comm);
    return 0;
}

