#include "ghezi.h"

void invalid_command(){
    perror("please enter a valid commnad!");
}

int remove_prefix(char *av, char *pat){
    int len = strlen(pat);
    if(strlen(av) < len)
        return 1;
    for(int i = 0; i < len; i++)
        if(av[i] != pat[i])
            return 1;
    int len2 = strlen(av);
    for(int i = 0; i <= len2 - len; i++)
        av[i] = av[i + len];
    return 0;
}

int chdir_ghezi(){
    char tmp_cwd[1024];
    bool exists = false;
    struct dirent *entry;
    do {
        DIR *dir = opendir(".");
        if (dir == NULL) {
            perror("Error opening current directory");
            return 2;
        }
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".ghezi") == 0){
                exists = true;
                if(chdir(".ghezi") != 0)
                    return 2;
                break;
            }
        }
        closedir(dir);
        if(exists)
            break;
        // update current working directory
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL) 
            return 2;

        // change cwd to parent
        if (strcmp(tmp_cwd, "/") != 0) {
            if (chdir("..") != 0) return 2;
        }

    } while (strcmp(tmp_cwd, "/") != 0);

    return !exists;
}

int main(int argc, char *argv[]) {
    
    if (argc < 2)
        return invalid_command(), 1;

    if (!strcmp(argv[1], "init"))
        return run_init(argc, argv);

    if(!strcmp(argv[1], "config")){
        if (argc < 3)
            return invalid_command(), 1;
        if(!strcmp(argv[2], "reset"))
            return reset_general_config();
        if (argc < 4)
            return invalid_command(), 1;
        if(!strcmp(argv[2], "-global")){
            if (argc < 5)
                return invalid_command(), 1;
            if(!strcmp(argv[3], "user.name"))
                return update_general_config(argv[4], 0);
            if(!strcmp(argv[3], "user.email"))
                return update_general_config(argv[4], 1);
            return invalid_command(), 1;
        }
        if(chdir_ghezi())
            return perror("no repo found!"), 1;
        if(!strcmp(argv[2], "user.name"))
            return update_config_single(argv[3], 0);
        if(!strcmp(argv[2], "user.email"))
            return update_config_single(argv[3], 1);
    }

    return invalid_command(), 0;
}