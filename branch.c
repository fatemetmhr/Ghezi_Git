#include "ghezi.h"

int make_branch(const char *name){
    if(get_silent())
        return 0;
    char keep_cwd[1024];
    if(getcwd(keep_cwd, sizeof(keep_cwd)) == NULL)
        return 1;
    if(chdir_ghezi())
        return 1;
    char path[1024];
    strcpy(path, "branch");
    add_to_string(path, "/", name);
    FILE *f = fopen(path, "r");
    if(f != NULL){
        if(chdir(keep_cwd))
            return 1;
        return fprintf(stderr, "branch %s already exist", name), 0;
    }
    f = fopen(path, "w");
    fclose(f);
    char cur_branch[1024];
    f = fopen(branch_name, "r");
    fscanf(f, "%s", cur_branch);
    fclose(f);
    f = fopen(branch_name, "w");
    fprintf(f, "%s", name);
    fclose(f);

    if(copy_file(stage_name, "branch_tmp_stage.txt"))
        return 1;
    f = fopen(stage_name, "w");
    fclose(f);

    if(commit(" ", true, true))
        return 1;
    
    f = fopen(branch_name, "w");
    fprintf(f, "%s", cur_branch);
    fclose(f);
    if(copy_file("branch_tmp_stage.txt", stage_name))
        return 1;
    remove("branch_tmp_stage.txt");
    if(chdir(keep_cwd))
        return 1;
    return 0;
}

int show_all_branchs(){
    if(get_silent())
        return 0;
    char keep_cwd[1024];
    if(getcwd(keep_cwd, sizeof(keep_cwd)) == NULL)
        return 1;
    if(chdir_ghezi() || chdir("branch"))
        return 1;
    struct dirent *entry;
    DIR *dir = opendir(".");
    if(dir == NULL)
        return 1;
    while((entry = readdir(dir)) != NULL) if(entry->d_type != DT_DIR)
        printf("%s\n", entry->d_name);
    closedir(dir);
    if(chdir(keep_cwd))
        return 1;
    return 0;
}