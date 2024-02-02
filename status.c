#include "ghezi.h"

bool status(bool silent){ // status of whole repo -> error returnrs 1
    char *spath = get_stage_path();
    char *tpath = get_track_path();
    char *head_path = get_head_path();
    bool re = false;

    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        return runtime_in_function("status"), 1;
    FILE *track = fopen(tpath, "r");
    if(track == NULL)
        return runtime_in_function("status"), 1;
    char s[1024];
    while(fscanf(track, "%s \n", s) > 0){
        char *last_stage = find_in_map(head_path, s);
        bool is_staged = strlen(find_in_map(spath, s));
        int sts = -1; // sts = 0 -> A, 1 -> M, 2 -> D, -1 -> no change
        if(is_deleted(s)){
            if(strcmp(last_stage, "NULL"))
                sts = 2;
            else
                sts = -1;
        }
        else if(strlen(last_stage)){
            if(are_diff(last_stage, s))
                sts = 1;
            else
                sts = -1;
        }
        else
            sts = 0;
        re |= (sts >= 0);
        if(!silent && sts >= 0)
            printf("file %s: %c%c\n", s, is_staged ? '+' : '-', sts == 0 ? 'A' : (sts == 1 ? 'M' : 'D'));
    }
    fclose(track);
    if(chdir_ghezi() || chdir(".."))
        return runtime_in_function("status"), 1;
    re |= find_untracking_files(silent);
    if(chdir(cwd))
        return runtime_in_function("status"), 1;
    return re;    
}

// in code akharin yadegare 18 salegime! tavalodam mobarak kheili bi dalil :)

bool find_untracking_files(bool silent){
    char *tpath = get_track_path();
    struct dirent *entry;
    DIR *dir = opendir(".");
    bool re = false;
    if(dir == NULL)
        return runtime_in_function("find_untracking_files"), 1;
    char *path;
    while((entry = readdir(dir)) != NULL) if(is_allowed_name(entry->d_name)){
        if(entry->d_type == DT_DIR){
            if(chdir(entry->d_name))
                return runtime_in_function("find_untracking_files"), 1;
            re |= find_untracking_files(silent);
            if(chdir(".."))
                return runtime_in_function("find_untracking_files"), 1;
        }
        else if(!is_in_file(tpath, (path = abs_path(entry->d_name)))){
            re = true;
            if(!silent)
                printf("file %s: -A\n", path);
        }
    }
    closedir(dir);
    return re;
}