#include "ghezi.h"

bool silent = false;

bool get_silent(){
    return silent;
}

int main(int argc, char *argv[]) {
    
    if (argc < 2)
        return invalid_command(), 1;

    if(!strcmp(argv[1], "-SILENT")){
        silent = true;
        for(int i = 2; i < argc; i++)
            argv[i - 1] = argv[i];
        argc--;
    }

    if(!silent && argc == 2){
        char *command = find_in_map_with_space(string_concat(general_configs_path, "/", general_alias), argv[1]);
        if(strlen(command))
            return system(string_concat2("ghezi ", command));
    }

    if(!strcmp(argv[1], "init")){
        if(argc > 2)
            return invalid_command(), 1;
        return run_init(argc, argv);
    }

    if(!strcmp(argv[1], "config")){
        if (argc < 3)
            return invalid_command(), 1;
        if(!strcmp(argv[2], "reset"))
            return reset_general_config();
        if (argc < 4)
            return invalid_command(), 1;
        if(!strcmp(argv[2], "-global")){
            if (argc != 5)
                return invalid_command(), 1;
            if(!strcmp(argv[3], "user.name"))
                return update_general_config(argv[4], 0);
            if(!strcmp(argv[3], "user.email"))
                return update_general_config(argv[4], 1);
            if(!remove_prefix(argv[3], "alias.")){
                if(!is_valid_command(argv[4])){
                    if(silent)
                        return 1;
                    return fprintf(stderr, "Command \"%s\" isn't valid, can't make alias!", argv[4]), 0;
                }
                return set_general_alias(argv[3], argv[4]);
            }
            return invalid_command(), 1;
        }
        if(!silent && chdir_ghezi())
            return fprintf(stderr, "no repo found! use -global flag if you want to change general configs\n"), 0;
        if(argc != 4)
            return invalid_command(), 1;
        if(!strcmp(argv[2], "user.name"))
            return update_config_single(argv[3], 0);
        if(!strcmp(argv[2], "user.email"))
            return update_config_single(argv[3], 1);
        if(!remove_prefix(argv[2], "alias.")){
            if(!is_valid_command(argv[3])){
                if(silent)
                    return 1;
                return fprintf(stderr, "Command \"%s\" isn't valid, can't make alias!", argv[3]), 0;
            }
            return set_alias(argv[2], argv[3]);
        }
        return invalid_command(), 1;
    }

    // check if any ghezi repo exists
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;
    if(!silent){
        int res;
        if((res = chdir_ghezi()) == 1)
            return fprintf(stderr, "no ghezi repo found!"), 0;
        else if(res)
            return 1;
        if(chdir(cwd))
            return 1;
    }
    
    if(!silent && argc == 2){
        char *command = find_in_map_with_space(string_concat(get_ghezi_wd(), "/", alias_name), argv[1]);
        if(strlen(command))
            return system(string_concat2("ghezi ", command));
    }

    if(!strcmp(argv[1], "add")){
        if(argc < 3)
            return invalid_command(), 1;
        if(!strcmp(argv[2], "-n")){
            if(argc != 4)
                return invalid_command(), 1;
            return show_stage_status_recursive("~ ", get_num(argv[3]));
        }
        if(!strcmp(argv[2], "-redo")){
            if(argc != 3)
                return invalid_command(), 1;
            return redo_add();
        }
        if(silent)
            return 0;
        int st = 2;
        if(!strcmp(argv[2], "-f"))
            st = 3;
        else if(argc != 3)
            return invalid_command(), 0;
        bool wild = false;
        for(int i = 0; i < argc; i++)
            wild |= is_wildcard(argv[i]);
        if(wild)
            return system(string_concat_master(argc, argv, 2, "-f"));
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
            return invalid_command(), 1;
        if(silent)
            return 0;
        bool wild = false;
        for(int i = 0; i < argc; i++)
            wild |= is_wildcard(argv[i]);
        if(!strcmp(argv[2], "-undo")){
            if(argc != 3)
                return invalid_command(), 0;
            return shift_stage_history(-1);
        }
        int st = 2;
        if(!strcmp(argv[2], "-f"))
            st = 3;
        else if(argc != 3)
            return invalid_command(), 0;
        if(wild)
            return system(string_concat_master(argc, argv, 2, "-f"));
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
            return invalid_command(), 1;
        if(silent)
            return 0;
        status(false);
        return 0;
    }

    if(!strcmp(argv[1], "commit")){
        if(argc < 3 || argc > 5)
            return invalid_command(), 1;
        if(argc < 4){
            if(silent)
                return 1;
            return fprintf(stderr, "please set a message(or shortcut) for your commit\n"), 0;
        }
        bool forced = false;
        if(!strcmp(argv[2], "-f")){
            forced = true;
            for(int i = 3; i < argc; i++)
                argv[i - 1] = argv[i];
            argc--;
        }

        if(argc > 4)
            return invalid_command(), 1;
        if(!strcmp(argv[2], "-m")){
            if(strlen(argv[3]) > MAX_COMMIT_MESSAGE_SIZE){
                if(silent)
                    return 1;
                return fprintf(stderr, "too long message!\n"), 0;
            }
            if(!forced && !silent && !is_in_head())
                return fprintf(stderr, "Commiting is only available in the HEAD of a branch!\n"), 0;
            return commit(argv[3], forced, false);
        }
        if(!strcmp(argv[2], "-s")){
            char *msg = find_short_cut(argv[3]);
            if(!silent && msg[0] == '\0')
                return fprintf(stderr, "shortcut %s does not exist\n", argv[3]), 0;
            if(!forced && !silent && !is_in_head())
                return fprintf(stderr, "Commiting is only available in the HEAD of a branch!\n"), 0;
            return commit(msg, forced, false);
        }
        return invalid_command(), 1;
    }

    if(!strcmp(argv[1], "set")){
        if(argc != 6 || strcmp(argv[2], "-m") || strcmp(argv[4], "-s"))
            return invalid_command(), 1;
        if(strlen(argv[3]) > MAX_COMMIT_MESSAGE_SIZE){
            if(silent)
                return 1;
            return fprintf(stderr, "too long commit message!\n"), 0;
        }
        return set_message_shortcut(argv[3], argv[5]);
    }

    if(!strcmp(argv[1], "remove")){
        if(argc != 4 || strcmp(argv[2], "-s"))
            return invalid_command(), 1;
        return remove_message_shortcut(argv[3]);
    }

    if(!strcmp(argv[1], "replace")){
        if(argc != 6 || strcmp(argv[2], "-m") || strcmp(argv[4], "-s"))
            return invalid_command(), 1;
        if(strlen(argv[3]) > MAX_COMMIT_MESSAGE_SIZE){
            if(silent)
                return 1;
            return fprintf(stderr, "too long commit message!\n"), 0;
        }
        return replace_message_shortcut(argv[3], argv[5]);
    }

    if(!strcmp(argv[1], "branch")){
        if(argc > 3 || argc < 2)
            return invalid_command(), 1;
        if(argc == 2)
            return show_all_branchs();
        return make_branch(argv[2]);
    }

    if(!strcmp(argv[1], "log")){
        if(argc < 3)
            return show_all_logs((int)1e9);
        if(!strcmp(argv[2], "-search")){
            int n = argc - 3;
            char **words = (char **)malloc(n * sizeof(char *));
            for(int i = 0; i < n; i++)
                words[i] = argv[3 + i];
            return show_all_word_match_commits(n, words);
        }
        if(argc != 4)
            return invalid_command(), 1;
        if(!strcmp(argv[2], "-n"))
            return show_all_logs(get_num(argv[3]));
        if(!strcmp(argv[2], "-branch"))
            return show_all_branch_commits(argv[3]);
        if(!strcmp(argv[2], "-author"))
            return show_all_personal_commits(argv[3]);
        if(!strcmp(argv[2], "-since"))
            return show_all_during_commits(make_tm_from_date(argv[3]), make_tm_from_date("987684/0/0"));
        if(!strcmp(argv[2], "-before"))
            return show_all_during_commits(make_tm_from_date("0/0/0"), make_tm_from_date(argv[3]));
        return invalid_command(), 1;
    }

    if(!strcmp(argv[1], "checkout")){
        bool forced = false;
        if(argc >= 2 && !strcmp(argv[2], "-f")){
            forced = true;
            for(int i = 3; i < argc; i++)
                argv[i - 1] = argv[i];
            argc--;
        }
        if(argc != 3)
            return invalid_command(), 1;
        if(!forced && !silent && status(true))
            return printf("Some files has been changed but not commited. Checkout failed!\n"), 0;
        if(!strcmp(argv[2], "HEAD"))
            return checkout_to_head(0);
        if(!remove_prefix(argv[2], "HEAD-")){
            int x = get_num(argv[2]);
            if(x == -1)
                return invalid_command(), 0;
            return checkout_to_head(x);
        }
        if(is_commit_id_valid(argv[2]))
            return checkout_to_commit(argv[2]);
        return checkout_to_branch(argv[2]);
    }
    
    if(!strcmp(argv[1], "revert")){
        char *msg = malloc(1024);
        msg[0] = '\0';
        if(argc < 3)
            return invalid_command(), 1;
        if(!strcmp(argv[2], "-n")){
            if(argc != 4)
                return invalid_command(), 1;
            if(!silent && status(true))
                return printf("Some files has been changed but not commited. Revert failed!\n"), 0;
            if(!silent && !is_in_head())
                return fprintf(stderr, "Reverting is only available in the HEAD of a branch!\n"), 0;
            return revert_to_commit(argv[3], msg, false);
        }
        if(!strcmp(argv[2], "-m")){
            if(argc != 5)
                return invalid_command(), 1;
            strcpy(msg, argv[3]);
            argc = 3;
            argv[2] = argv[4];
            if(strlen(msg) > MAX_COMMIT_MESSAGE_SIZE){
                if(silent)
                    return 1;
                return fprintf(stderr, "too long message!"), 0;
            }
        }
        if(argc != 3)
            return invalid_command(), 1;
        if(!silent && status(true))
            return printf("Some files has been changed but not commited. Revert failed!\n"), 0;
        if(!silent && !is_in_head())
            return fprintf(stderr, "Reverting is only available in the HEAD of a branch!\n"), 0;
        if(!remove_prefix(argv[2], "HEAD-")){
            int x = get_num(argv[2]);
            if(x == -1)
                return invalid_command(), 1;
            return revert_to_head(x, msg);
        }
        return revert_to_commit(argv[2], msg, true);
    }

    if(!strcmp(argv[1], "tag")){
        if(argc == 2)
            return show_all_tags();
        if(!strcmp(argv[2], "-a")){
            if(argc < 4)
                return invalid_command(), 1;
            bool overwrite = false;
            char *msg = malloc(1024);
            msg[0] = ' ';
            msg[1] = '\0';
            char *id = malloc(1024);
            id[0] = '\0';
            if(argc > 4 && !strcmp(argv[4], "-m")){
                if(argc < 6)
                    return invalid_command(), 1;
                strcpy(msg, argv[5]);
                for(int i = 6; i < argc; i++)
                    argv[i - 2] = argv[i];
                argc -= 2;
            }
            if(argc > 4 && !strcmp(argv[4], "-c")){
                if(argc < 6)
                    return invalid_command(), 1;
                strcpy(id, argv[5]);
                for(int i = 6; i < argc; i++)
                    argv[i - 2] = argv[i];
                argc -= 2;
            }
            if(argc > 4 && !strcmp(argv[4], "-f")){
                overwrite = true;
                for(int i = 5; i < argc; i++)
                    argv[i - 1] = argv[i];
                argc--;
            }
            if(argc != 4)
                return invalid_command(), 1;
            if(silent)
                return 0;
            if(!strlen(id))
                id = get_current_commit_id();
            return add_tag(argv[3], id, msg, overwrite);

        }
        if(!strcmp(argv[2], "show")){
            if(argc != 4)
                return invalid_command(), 1;
            return print_tag(argv[3]);

        }
        return invalid_command(), 1;
    }

    return invalid_command(), 1;
}