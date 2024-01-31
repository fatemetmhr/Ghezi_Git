#include "ghezi.h"

int add_file(char *name){ // in this directory, will be return here as well, file didn't found -> return 0;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) 
        return 1;
    
    // find real path
    char fpath[1024];
    strcpy(fpath, cwd);
    add_to_string(fpath, "/", name);

    // remove previouse added
    if(reset_file(name))
        return 1;
    

    FILE *f = fopen(name, "r");    
    
    if(chdir_ghezi())
        return 1;

    // if file has been removed or just does not exist it should be mentioned
    if(f == NULL){
        FILE *stage = fopen(stage_name, "a");
        if(stage == NULL)
            return 1;
        fprintf(stage, "%s NULL\n", fpath);
        return fprintf(stderr, "file or directory %s doesn't exist or has been deleted!", name), 0;
    }
    
    // find new name for copy of the file in source
    char new_name[100];
    FILE *nname = fopen(new_name_keeper, "r");
    if(nname == NULL)
        return 1;
    fscanf(nname, "%s", new_name);
    fclose(nname);
    if((nname = fopen(new_name_keeper, "r")) == NULL)
        return 1;
    int x;
    fscanf(nname, "%d", &x);
    fclose(nname);
    if((nname = fopen(new_name_keeper, "w")) == NULL)
        return 1;
    fprintf(nname, "%d", ++x);
    fclose(nname);

    // make a copy of the file
    char cppath[1024];
    if(getcwd(cppath, sizeof(cppath)) == NULL)
        return 1;
    add_to_string(cppath, "/source/", new_name);
    copy_file(fpath, cppath);

    // add it's path to stage keeper
    FILE *stage = fopen(stage_name, "a");
    if(stage == NULL)
        return 1;
    fprintf(stage, "%s %s\n", fpath, cppath);
    fclose(stage);

    if(chdir(cwd) != 0)
        return 1;
    return 0;
}

int add_dir(){ // add all files in this directory
    struct dirent *entry;
    DIR *dir = opendir(".");
    if(dir == NULL)
        return 1;
    while((entry = readdir(dir)) != NULL){
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        if(entry->d_type == DT_DIR){
            if(chdir(entry->d_name) != 0)
                return 1;
            if(add_dir())
                return 1;
            if(chdir(".."))
                return 1;
        }
        else if(add_file(entry->d_name))
            return 1;
    }
    closedir(dir);
    return 0;
}

int show_stage_status(){
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;
    
    // get all staged files in this directory
    if(chdir_ghezi())
        return 1;
    FILE *stages = fopen(stage_name, "r");
    if(stages == NULL)
        return 1;
    char *name[1024], *rl_path[1024], *cp_path[1024];
    int cnt = 0;
    char *rl = malloc(1024);
    char *cp = malloc(1024);
    while(fscanf(stages, "%s %s\n", rl, cp) > 0){
        char *file_name = make_par_dir(rl);
        if(strcmp(rl, cwd))
            continue;
        if(cnt == 1024)
            return fprintf(stderr, "more thatn 1024 files found!"), 1;
        name[cnt] = file_name;
        rl_path[cnt] = malloc(1024);
        strcpy(rl_path[cnt], rl);
        cp_path[cnt] = malloc(1024);
        strcpy(cp_path[cnt], cp);
        cnt++;
    }
    fclose(stages);
    if(chdir(cwd))
        return 1;
    
    // find unchaned files
    struct dirent *entry;
    DIR *dir = opendir(".");
    if(dir == NULL)
        return 1;
    while((entry = readdir(dir)) != NULL) if(entry -> d_type != DT_DIR){
        int re = 0; // re = 0 -> not staged, 1 -> staged but changed, 2 -> staged
        for(int i = 0; i < cnt; i++) if(!strcmp(entry -> d_name, name[i])){
            re = 1;
            if(!are_diff(cp_path[i], name[i]))
                re = 2;
            break;
        }
        printf("%s : ", entry -> d_name);
        if(!re)
            printf("not staged\n");
        else if(re == 1)
            printf("staged but modefied\n");
        else
            printf("staged\n");
    }
    return 0;
}


int reset_file(char *name){
    char cwd[1024], file[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) 
        return 1;
    strcpy(file, cwd);
    add_to_string(file, "/", name);

    if(chdir_ghezi())
        return 9;
    FILE *stage = fopen(stage_name, "r");
    FILE *nstage = fopen("tmp.txt", "w");
    if(stage == NULL || nstage == NULL)
        return 87;
    char rl[1024], cp[1024];
    while(fscanf(stage, "%s %s\n", rl, cp) > 0){
        if(strcmp(rl, file))
            fprintf(nstage, "%s %s\n", rl, cp);
    }
    fclose(stage);
    fclose(nstage);
    remove(stage_name);
    if(rename_file("tmp.txt", stage_name))
        return 8;

    if(chdir(cwd))
        return 1;
    return 0;
}
