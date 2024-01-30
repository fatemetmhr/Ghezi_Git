#include "ghezi.h"


int update_config(char *name, char *email){ // should be called in .ghezi
    FILE *conf = fopen("config.txt", "w");
    if(conf == NULL)
        return 1;
    fprintf(conf, "%s %s\n", name, email);
    fclose(conf);

    /*
    char *new_configs = (char *)malloc(MAX_CONFIG_NAME);
    strcpy(new_configs, name);
    int len = strlen(new_configs);
    new_configs[len] = ' ';
    strcpy(new_configs + len + 1, email);
    len = strlen(new_configs);
    new_configs[len] = '\n';
    new_configs[len + 1] = '\0';

    if (chdir(general_configs_path) != 0) 
        return 1;
    FILE *configs = fopen(general_configs_name, "r");
    FILE *updated = fopen(general_configs_name, "w");
    char *s = malloc(MAX_CONFIG_NAME);
    while(!feof(configs)){
        fgets(s, MAX_CONFIG_NAME, configs);
        if(!strcmp(s, pre_configs))
            s = new_configs;
        fprintf(updated, "%s", s);
    }
    fclose(configs);
    remove(general_configs_name);
    updated = (updated, general_configs_name);
    fclose(updated);

    free(s);
    free(pre_configs);
    free(new_configs);
    */

    return 0;
}

int update_config_single(char *infor, int ty){ // ty = 0 -> infor is name   
    FILE *cur = fopen("config.txt", "r");
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
    fprintf(stderr, "%s %d\n", infor, ty);
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
    while(!feof(gconf)){
        fgets(path, sizeof(path), gconf);
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
    if(chdir(general_configs_path) != 0)
        return 1;
    FILE *f = fopen(general_configs_name, "w");
    if(f == NULL)
        return 1;
    fclose(f);
    remove(last_general_name);
    remove(last_general_email);
    return 0;
}