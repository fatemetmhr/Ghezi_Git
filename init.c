#include "ghezi.h"

int run_init(int argc, char * const argv[]) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) 
        return 1;
    int res = chdir_ghezi();
    if (res == 1){
        if(chdir(general_configs_path) != 0)
            return 1;
        // general config checking

        FILE *gname = fopen(last_general_name, "r");
        FILE *gemail = fopen(last_general_email, "r");
        if(gname == NULL)
            return fprintf(stderr, "please settup a general username befor creating ghezi"), 1;
        if(gemail == NULL)
            return fprintf(stderr, "please settup a general email befor creating ghezi"), 1;
        char name[1024], email[1024];
        fscanf(gname, "%s", name);
        fscanf(gemail, "%s", email);
        fclose(gname);
        fclose(gemail);
        if(chdir(cwd) != 0)
            return  1;
        
        // making repo
        if (mkdir(".ghezi", 0755) != 0) 
            return 2;
        printf("ghezi has been created successfully!");
        if(chdir(".ghezi") != 0)
            return 1;

        // making source folder and counter file
        if(mkdir("source", 0755) != 0)
            return 1;
        FILE *f = fopen(new_name_keeper, "w");
        if(f == NULL)
            return 1;
        fprintf(f, "%d", 0);
        fclose(f);

        // making stage keeper and tracker
        for(int i = 0; i < MAX_STAGE_HISTORY; i++){
            f = fopen(stage_history[i], "w");
            fclose(f);
        }
        f = fopen(tracker_name, "w");
        fclose(f);

        char cur[1024];
        if (getcwd(cur, sizeof(cur)) == NULL) 
            return 1;

        // add path to general paths
        if (chdir(general_configs_path) != 0)
            return 1;
        FILE *gconf = fopen(general_configs_name, "a");
        if(gconf == NULL)
            return fprintf(stderr, "error while opening general configs file"), 1;
        fprintf(gconf, "%s\n", cur);
        fclose(gconf);


        if(chdir(cwd) != 0)
            return 1;
        if(chdir(".ghezi") != 0)
            return 1;
        
        // set local configs
        return update_config(name, email);
    }else if(res == 0) {
        printf("ghezi repository has already initialized\n");
    }
    else
        return 1;
    return 0;
}