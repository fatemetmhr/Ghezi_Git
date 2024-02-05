#include "ghezi.h"

int run_init(int argc, char * const argv[]) {
    if(get_silent())
        return 0;
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
            return fprintf(stderr, "please settup a general username befor creating ghezi\n"), 1;
        if(gemail == NULL)
            return fprintf(stderr, "please settup a general email befor creating ghezi\n"), 1;
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
        printf("ghezi has been created successfully!\n");
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

        // making alias files
        f = fopen(alias_name, "w");
        fclose(f);

        //making commit directory and counter file and HEAD file and shortcut keeper file and branch name keeper
        if(mkdir("commits", 0755))
            return 1;
        f = fopen(new_commit_id_keeper, "w");
        if(f == NULL)
            return 1;
        fprintf(f, "%d", 0);
        fclose(f);
        f = fopen(head_name, "w");
        fclose(f);
        f = fopen(msg_shortcuts, "w");
        fclose(f);
        f = fopen(branch_name, "w");
        fprintf(f, "master");
        fclose(f);
        f = fopen(all_commits, "w");
        fclose(f);

        // making branch folder
        if(mkdir("branch", 0755))
            return 1;
        f = fopen("branch/master", "w");
        fclose(f);

        // making stage keeper and tracker
        for(int i = 0; i < MAX_STAGE_HISTORY; i++){
            f = fopen(stage_history[i], "w");
            fclose(f);
        }
        f = fopen(tracker_name, "w");
        fclose(f);

        // set cur commit id
        f = fopen(last_commit, "w");
        fprintf(f, "%d", 0);
        fclose(f);

        // creat file for tag and a folder to keep them
        f = fopen(tag_name, "w");
        fclose(f);
        f = fopen(commits_with_tag_name, "w");
        fclose(f);
        if(mkdir("tag", 0755))
            return 1;
        

        char cur[1024];
        if (getcwd(cur, sizeof(cur)) == NULL) 
            return 1;

        // add path to general paths
        if (chdir(general_configs_path) != 0)
            return 1;
        FILE *gconf = fopen(general_configs_name, "a");
        if(gconf == NULL)
            return fprintf(stderr, "error while opening general configs file\n"), 1;
        fprintf(gconf, "%s\n", cur);
        fclose(gconf);



        if(chdir(cwd) != 0)
            return 1;
        if(chdir(".ghezi") != 0)
            return 1;
        
        // set local configs
        if(update_config(name, email))
            return 1;
        return commit(" ", true, true);
    }else if(res == 0) {
        printf("ghezi repository has already initialized\n");
    }
    else
        return 1;
    return 0;
}