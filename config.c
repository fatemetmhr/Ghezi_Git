#include "ghezi.h"


int update_config(char *name, char *email){ // should be called in .ghezi
    if(get_silent())
        return 0;
    FILE *conf = fopen(config_name, "w");
    if(conf == NULL)
        return 1;
    fprintf(conf, "%s %s\n", name, email);
    fclose(conf);
    return 0;
}

int update_config_single(char *infor, int ty){ // ty = 0 -> infor is name 
    if(get_silent())
        return 0;  
    FILE *cur = fopen(config_name, "r");
    if(cur == NULL)
        return 34;
    char *s = (char *)malloc(MAX_CONFIG_NAME);
    fscanf(cur, "%s", s);
    if(!ty)
        fscanf(cur, "%s", s);
    fclose(cur);
    int res = update_config(ty ? s : infor, ty ? infor : s);
    free(s);
    return res;
}

int update_general_config(char *infor, int ty){ // ty = 0 -> infor is name
    if(get_silent())
        return 0;
    if (chdir(general_configs_path) != 0) 
        return 1;
    FILE *f;
    if(!ty)
        f = fopen(last_general_name, "w");
    else
        f = fopen(last_general_email, "w");
    if(f == NULL)
        return 1;
    fprintf(f, "%s", infor);
    fclose(f);
    FILE *gconf = fopen(general_configs_name, "r");
    char path[MAX_PATH_SIZE];
    while(fgets(path, sizeof(path), gconf)){
        path[strlen(path) - 1] = '\0';
        if(chdir(path) != 0){
            fprintf(stderr, "error while trying to open project %s skipping..\n", path);
            continue;
        }
        if(update_config_single(infor, ty))
            return 1;
    }
    fclose(gconf);
    return 0;
}

int reset_general_config(){
    if(get_silent())
        return 0;
    if(chdir(general_configs_path) != 0)
        return 1;
    FILE *f = fopen(general_configs_name, "w");
    if(f == NULL)
        return 1;
    fclose(f);
    remove(last_general_name);
    remove(last_general_email);
    remove(general_alias);
    return 0;
}

int set_alias(char *alias, char *command){
    if(get_silent())
        return 0;
    if(chdir_ghezi())
        return 1;
    char *av = find_in_map_with_space(alias_name, alias);
    if(strlen(av))
        return fprintf(stderr, "alias already exist for \"%s\"", av), 0;
    FILE *f = fopen(alias_name, "a");
    fprintf(f, "%s %s\n", alias, command);
    fclose(f);
    return 0;
}

int set_general_alias(char *alias, char *command){
    if(get_silent())
        return 0;
    if(chdir(general_configs_path))
        return 1;
    char *av = find_in_map_with_space(general_alias, alias);
    if(strlen(av))
        return fprintf(stderr, "alias already exist for \"%s\"", av), 0;
    FILE *f = fopen(general_alias, "a");
    fprintf(f, "%s %s\n", alias, command);
    fclose(f);
    f = fopen(general_configs_name, "r");
    if(f == NULL)
        return 1;
    char path[1024];
    while(fscanf(f, "%s \n", path) > 0){
        if(chdir(path))
            return 1;
        if(set_alias(alias, command))
            return 1;
    }
    fclose(f);
    return 0;
}