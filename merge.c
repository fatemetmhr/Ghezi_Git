#include "ghezi.h"

int merge_branch(const char *brn1, const char *brn2){ // new branch name will be brc1 + brc2
    if(get_silent())
        return 0;
    if(chdir_ghezi())
        return 1;
    
    if(is_in_file(merged_branchs, brn1))
        return fprintf(stderr, "branch %s already merged with another branch", brn1), 0;
    if(is_in_file(merged_branchs, brn2))
        return fprintf(stderr, "branch %s already merged with another branch", brn2), 0;

    FILE *f = fopen(string_concat2("branch/", brn1), "r");
    if(f == NULL)
        return fprintf(stderr, "branch %s does not exist!", brn1), 0;
    char head1[1024], head2[1024];
    fscanf(f, "%s", head1);
    fclose(f);
    f = fopen(string_concat2("branch/", brn2), "r");
    if(f == NULL)
        return fprintf(stderr, "branch %s does not exist!", brn2), 0;
    fscanf(f, "%s", head2);
    fclose(f);

    if(commit_diff_checker(head1, head2, false, true, true, string_concat("in branch ", brn1, " "), string_concat("in branch ", brn2, " ")))
        return fprintf(stderr, "conflict detected, merge can't be done"), 0;

    char *new_branch = string_concat(brn1, "+", brn2);

    f = fopen(merged_branchs, "a");
    if(f == NULL)
        return 1;
    fprintf(f, "%s\n%s\n", brn1, brn2);
    fclose(f);

    char keep_branch[1024];

    f = fopen(branch_name, "r");
    if(f == NULL)
        return 1;
    fscanf(f, "%s", keep_branch);
    fclose(f);

    f = fopen(branch_name, "w");
    fprintf(f, "%s", new_branch);
    fclose(f);

    char *path = string_concat2("branch/", new_branch);
    f = fopen(path, "w");
    char *id = creat_new_commit(" ", true);
    fprintf(f, "%s\n", id);
    fclose(f);
    f = fopen(string_concat("commits/", string_concat2(id, "/"), stage_name), "w");
    fclose(f);
    f = fopen(string_concat("commits/", string_concat2(id, "/"), commit_paths), "w");
    FILE *f2 = fopen(string_concat("commits/", string_concat2(head1, "/"), commit_paths), "r");
    if(f2 == NULL)
        return 1;
    char rl[1024], cp[1024];
    while(fscanf(f2, "%s %s \n", rl, cp) > 0) if(strcmp(cp, "NULL"))
        fprintf(f, "%s %s\n", rl, cp);
    fclose(f2);
    f2 = fopen(string_concat("commits/", string_concat2(head2, "/"), commit_paths), "r");
    if(f2 == NULL)
        return 1;
    while(fscanf(f2, "%s %s \n", rl, cp) > 0){
        char *res = find_in_map(string_concat("commits/", string_concat2(head1, "/"), commit_paths), rl);
        if(!strlen(res) || !strcmp(res, "NULL"))
            fprintf(f, "%s %s\n", rl, cp);
    }
    fclose(f2);
    fclose(f);

    f = fopen(branch_name, "w");
    fprintf(f, "%s", keep_branch);
    fclose(f);
    fprintf(stderr, "branch %s %s merged to a new branch named %s\n", brn1, brn2, new_branch);
    return 0;
}