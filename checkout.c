#include "ghezi.h"

int checkout_to_commit(char *id){
    if(get_silent())
        return 0;
    if(chdir_ghezi())
        return 1;
    FILE *cur_commit = fopen(last_commit, "w");
    fprintf(cur_commit, "%s", id);
    fclose(cur_commit);
    FILE *head = fopen(head_name, "w");
    if(chdir(".."))
        return 1;
    if(remove_all_here())
        return 1;
    if(chdir(string_concat(".ghezi/commits", "/", id)))
        return fprintf(stderr, "no such branch or commit id exist!\n"), 0;
    FILE *f = fopen(commit_paths, "r");
    if(f == NULL)
        return 1;
    char rl[1024], cp[1024];
    while(fscanf(f, "%s %s\n", rl, cp) > 0){
        fprintf(head, "%s %s\n", rl, cp);
        if(!strcmp(cp, "NULL"))
            continue;
        if(make_file(rl) || copy_file(cp, rl))
            return 1;
    }
    fclose(head);
    fclose(f);
    f = fopen(commit_branch, "r");
    if(f == NULL)
        return 1;
    char s[1024];
    fscanf(f, "%s", s);
    fclose(f);
    if(chdir("../.."))
        return 1;
    f = fopen(branch_name, "w");
    fprintf(f, "%s", s);
    fclose(f);
    return 0;
}

int checkout_to_branch(char *name){
    if(get_silent())
        return 0;
    if(chdir_ghezi() || chdir("branch"))
        return 1;
    FILE *f = fopen(name, "r");
    if(f == NULL)
        return fprintf(stderr, "no such branch or commit id exist!\n"), 0;
    char id[1024];
    fscanf(f, "%s", id);
    fclose(f);
    return checkout_to_commit(id);
}

int checkout_to_head(int n){
    if(get_silent())
        return 0;
    if(chdir_ghezi())
        return 1;
    FILE *f = fopen(branch_name, "r");
    if(f == NULL)
        return 1;
    char branch[1024], tmp[1024], id[1024];
    fscanf(f, "%s", branch);
    fclose(f);
    if(chdir("branch"))
        return 1;
    f = fopen(branch, "r");
    int keep = n;
    n++;
    while(n--){
        if(fscanf(f, "%s \n", id) <= 0)
            return fprintf(stderr, "less than %d commits exist in this branch!\n", keep), 0;
        
    }
    if(fscanf(f, "%s \n", tmp) <= 0)
        return fprintf(stderr, "less than %d commits exist in this branch!\n", keep), 0;
    fclose(f);
    return checkout_to_commit(id);
}

