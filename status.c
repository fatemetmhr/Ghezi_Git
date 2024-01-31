#include "ghezi.h"

int status(){ // status of while repo
    char *spath = get_ghezi_wd();
    char *tpath = malloc(2048);
    strcpy(tpath, spath);
    add_to_string(spath, "/", stage_name);
    add_to_string(tpath, "/", tracker_name);

    struct dirent *entry;
    DIR *dir = opendir(".");
    if (dir == NULL)
        return 1;
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;
    char *av[1024];
    bool mark[1024];
    memset(mark, 0, sizeof(mark));
    int n = 0;
    while ((entry = readdir(dir)) != NULL) if(is_allowed_name(entry->d_name)){
        if(entry->d_type == DT_DIR){
            if(chdir(entry->d_name))
                return 1;
            if(status())
                return 1;
            if(chdir(".."))
                return 1;
        }
        else{
            char *name = malloc(1024);
            strcpy(name, cwd);
            add_to_string(name, "/", entry->d_name);
            if(!is_in_file(tpath, name))
                printf("file %s : -A\n", name);
            else
                av[n++] = name;
        }
    }
    closedir(dir);

    FILE *track = fopen(tpath, "r");
    if(track == NULL)
        return 1;
    char s[1024];
    //debug("ok ok ");
    //print_cwd();
    while(fscanf(track, "%s \n", s) > 0){
        //debug(s);
        if(!is_file_here(cwd, s))
            continue;
        bool re = false;
        for(int i = 0; i < n; i++) 
            re |= (!strcmp(s, av[i]));
        if(!re){
            if(is_in_file(spath, s))
                printf("file %s : +D\n", s);
            else
                printf("file %s : -D\n", s);
        }
    }
    fclose(track);

    for(int i = 0; i < n; i++){
        char *cppath = find_in_map(spath, av[i]);
        if(strlen(cppath) == 0){
            cppath = find_in_map(cppath, av[i]);
            if(strlen(cppath) && are_diff(cppath, av[i]))
                printf("file %s : -M\n", av[i]);
            else
                printf("file %s : -A\n", av[i]);
        }
        else if(are_diff(cppath, av[i]))
            printf("file %s : +M\n", av[i]);
    }
    return 0;    
}

// in code akharin yadegare 18 salegime! tavalodam mobarak kheili bi dalil :)