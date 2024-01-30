#include "ghezi.h"

int run_init(int argc, char * const argv[]) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    int res = chdir_ghezi();
    if (res == 1){
        if(chdir(general_configs_path) != 0)
            return 1;
        FILE *gname = fopen(last_general_name, "r");
        FILE *gemail = fopen(last_general_email, "r");
        if(gname == NULL)
            return perror("please settup a general username befor creating ghezi"), 1;
        if(gemail == NULL)
            return perror("please settup a general email befor creating ghezi"), 1;
        char name[1024], email[1024];
        fscanf(gname, "%s", name);
        fscanf(gemail, "%s", email);
        fclose(gname);
        fclose(gemail);
        if(chdir(cwd) != 0)
            return  1;
        
        if (mkdir(".ghezi", 0755) != 0) 
            return 2;
        perror("ghezi has been created successfully!");
        if(chdir(".ghezi") != 0)
            return 1;
        char cur[1024];
        if (getcwd(cur, sizeof(cur)) == NULL) 
            return 1;
        if (chdir(general_configs_path) != 0)
            return 1;
        FILE *gconf = fopen(general_configs_name, "a");
        if(gconf == NULL)
            return perror("error while opening general configs file"), 1;
        fprintf(gconf, "%s\n", cur);
        fclose(gconf);
        if(chdir(cwd) != 0)
            return 1;
        if(chdir(".ghezi") != 0)
            return 1;
        return update_config(name, email);
    }else if(res == 0) {
        perror("ghezi repository has already initialized");
    }
    else
        return 1;
    return 0;
}