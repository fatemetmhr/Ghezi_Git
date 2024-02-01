#include "ghezi.h"

int main(int argc, char *argv[]) {
    
    if (argc < 2)
        return invalid_command(), 0;

    if (!strcmp(argv[1], "init")){
        if(argc > 2)
            return invalid_command(), 0;
        return run_init(argc, argv);
    }

    if(!strcmp(argv[1], "config")){
        if (argc < 3)
            return invalid_command(), 0;
        if(!strcmp(argv[2], "reset"))
            return reset_general_config();
        if (argc < 4)
            return invalid_command(), 0;
        if(!strcmp(argv[2], "-global")){
            if (argc < 5)
                return invalid_command(), 0;
            if(!strcmp(argv[3], "user.name"))
                return update_general_config(argv[4], 0);
            if(!strcmp(argv[3], "user.email"))
                return update_general_config(argv[4], 1);
            return invalid_command(), 0;
        }
        if(chdir_ghezi())
            return perror("no repo found!"), 1;
        if(!strcmp(argv[2], "user.name"))
            return update_config_single(argv[3], 0);
        if(!strcmp(argv[2], "user.email"))
            return update_config_single(argv[3], 1);
    }

    // check if any ghezi repo exists
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;
    int res;
    if((res = chdir_ghezi()) == 1)
        return fprintf(stderr, "no ghezi repo found!"), 0;
    else if(res)
        return 1;
    if(chdir(cwd))
        return 1;
    

    if(!strcmp(argv[1], "add")){
        if(argc < 3)
            return invalid_command(), 0;
        if(!strcmp(argv[2], "-n")){
            if(argc < 4)
                return invalid_command(), 0;
            return show_stage_status();
        }
        int st = 2;
        if(!strcmp(argv[2], "-f"))
            st = 3;
        if(shift_stage_history(1))
            return 1;
        while(st < argc){
            if(chdir(argv[st]) != 0){
                if(add_file(argv[st]))
                    return 1;
            }
            else{
                if(add_dir())
                    return 1;
                if(chdir(".."))
                    return 1;
            }
            st++;
        }
        return 0;
    }

    if(!strcmp(argv[1], "reset")){
        if(argc < 3)
            return invalid_command(), 0;
        if(!strcmp(argv[2], "-undo"))
            return shift_stage_history(-1);
        int st = 2;
        if(!strcmp(argv[2], "-f"))
            st = 3;
        while(st < argc){
            if(chdir(argv[st]) != 0){
                if(reset_file(argv[st]))
                    return 1;
            }
            else{
                if(reset_dir())
                    return 1;
                if(chdir(".."))
                    return 1;
            }
            st++;
        }
        return 0;
    }

    if(!strcmp(argv[1], "status")){
        if(argc > 2)
            return invalid_command(), 0;
        return status();
    }

    return invalid_command(), 0;
}