#include "ghezi.h"

int show_hook_list(){
    if(get_silent())
        return 0;
    char keep_cwd[1024];
    if(getcwd(keep_cwd, sizeof(keep_cwd)) == NULL)
        return 1;
    if(chdir_ghezi())
        return 1;

    FILE *f = fopen(hook_list, "r");
    char s[1024];
    printf("Available hooks in list:\n");
    while(fscanf(f, "%s \n", s) > 0)
        printf("%s\n", s);
    fclose(f);

    if(chdir(keep_cwd))
        return 1;
    return 0;
}

int show_all_hooks(){
    if(get_silent())
        return 0;
    printf("1. todo-check         : checks for TODO word in file\n");
    printf("2. eof-blank-space    : checks if file ends with whitespace\n");
    printf("3. format-check       : checks if file has valid format(.c .cpp .txt .mp3 .mp4 .wav .jpg)\n");
    printf("4. balance-braces     : checks if brackets are balanced\n");
    printf("5. indentation-check  : belive me, I have no idea what this hook will do, don't try it!\n");
    printf("6. static-error-check : checks for static error in a C/C++ file\n");
    printf("7. file-size-check    : checks if file's size is more than 5 Mb\n");
    printf("8. character-limit    : checks if file has more than 20000 characters\n");
    printf("9. time-limit         : checks if file's duration is more than 10 mins\n");
    return 0;
}

int add_hook(const char *hook){
    if(get_silent())
        return 0;
    if(chdir_ghezi())
        return 1;
    if(is_in_file(hook_list, hook))
        return fprintf(stderr, "hook %s already exists in list\n", hook), 0;
    FILE *f = fopen(hook_list, "a");
    if(f == NULL)
        return 1;
    fprintf(f, "%s\n", hook);
    fclose(f);
    return 0;
}

int remove_hook(const char *hook){
    if(get_silent())
        return 0;
    if(chdir_ghezi())
        return 1;
    if(!is_in_file(hook_list, hook))
        return fprintf(stderr, "hook %s doesn't exist in list\n", hook), 0;
    return remove_from_file(hook_list, hook);
}

int run_pre_commit(bool silent){
    if(get_silent())
        return 0;
    if(chdir_ghezi())
        return runtime_in_function("run_pre_commit"), 0;
    bool failed = false;
    char rl[1024], cp[1024];
    FILE *f1 = fopen(stage_name, "r");
    while(fscanf(f1, "%s %s\n", rl, cp) > 0){
        if(!strcmp(cp, "NULL"))
            continue;
        if(!silent)
            printf("file %s:\n", rl);
        FILE *f2 = fopen(hook_list, "r");
        char hook[1024];
        while(fscanf(f2, "%s \n", hook) > 0)
            failed |= run_pre_commit_for_hook(cp, get_file_format(rl), hook, silent);
        fclose(f2);
    }
    fclose(f1);
    return failed;
}

int run_pre_commit_for_hook(const char *file, const char *format, const char *hook, bool silent){
    if(!strcmp(hook, "todo-check"))
        return run_todo_hook(file, format, silent);
    if(!strcmp(hook, "eof-blank-space"))
        return run_eof_hook(file, format, silent);
    if(!strcmp(hook, "format-check"))
        return run_format_hook(file, format, silent);
    if(!strcmp(hook, "balance-braces"))
        return run_brackets_hook(file, format, silent);
    if(!strcmp(hook, "static-error-check"))
        return run_compile_hook(file, format, silent);
    if(!strcmp(hook, "file-size-check"))
        return run_size_hook(file, format, silent);
    if(!strcmp(hook, "character-limit"))
        return run_char_limit_hook(file, format, silent);
    return 0;
}

int run_todo_hook(const char *file, const char *format, bool silent){
    if(strcmp(format, ".cpp") && strcmp(format, ".c") && strcmp(format, ".txt")){
        if(!silent){
            printf("\033[1;33m");
            printf("todo-check......................................SKIPPED\n");
            printf("\033[1;0m");
        }
        return 0;
    }
    FILE *f = fopen(file, "r");
    if(f == NULL)
        return runtime_in_function("run_todo_hook"), 0;
    bool re = false;
    char s[MAX_LINE_SIZE];
    while(fgets(s, sizeof(s), f) && !re){
        int len = strlen(s);
        for(int i = 0; i + 3 < len; i++){
            char keep = s[i + 4];
            s[i + 4] = '\0';
            if(!strcmp(s + i, "TODO") && (i == 0 || is_white_space(s[i - 1])) && is_white_space(keep))
                re = true;
            s[i + 4] = keep;
        }
    }
    fclose(f);
    if(!silent){
        if(re){
            printf("\033[1;31m");
            printf("todo-check......................................FAILED\n");
            printf("\033[1;0m");
        }
        else{
            printf("\033[1;32m");
            printf("todo-check......................................PASSED\n");
            printf("\033[1;0m");
        }
    }
    return re;
}

int run_eof_hook(const char *file, const char *format, bool silent){
    if(strcmp(format, ".cpp") && strcmp(format, ".c") && strcmp(format, ".txt")){
        if(!silent){
            printf("\033[1;33m");
            printf("eof-blank-space................................SKIPPED\n");
            printf("\033[1;0m");
        }
        return 0;
    }
    FILE *f = fopen(file, "r");
    bool re = false;
    char s[MAX_LINE_SIZE];
    s[0] = '\0';
    int cnt = 0;
    while(fgets(s, sizeof(s), f))
        cnt++;
    fclose(f);
    int len = strlen(s);
    re = cnt && (len == 0 || is_white_space(s[len - 1]));
    if(!silent){
        if(re){
            printf("\033[1;31m");
            printf("eof-blank-space................................FAILED\n");
            printf("\033[1;0m");
        }
        else{
            printf("\033[1;32m");
            printf("eof-blank-space................................PASSED\n");
            printf("\033[1;0m");
        }
    }
    return re;
}

int run_format_hook(const char *file, const char *format, bool silent){
    bool re = strcmp(format, ".cpp") && strcmp(format, ".c") && strcmp(format, ".txt") && strcmp(format, ".mp3");
    re &= strcmp(format, ".mp4") && strcmp(format, ".wav") && strcmp(format, ".jpg");
    if(!silent){
        if(re){
            printf("\033[1;31m");
            printf("format-check...................................FAILED\n");
            printf("\033[1;0m");
        }
        else{
            printf("\033[1;32m");
            printf("format-check...................................PASSED\n");
            printf("\033[1;0m");
        }
    }
    return re;
}


int run_brackets_hook(const char *file, const char *format, bool silent){
    if(strcmp(format, ".cpp") && strcmp(format, ".c") && strcmp(format, ".txt")){
        if(!silent){
            printf("\033[1;33m");
            printf("balance-braces.................................SKIPPED\n");
            printf("\033[1;0m");
        }
        return 0;
    }
    FILE *f = fopen(file, "r");
    char s[MAX_LINE_SIZE];
    int cnt[2000];
    memset(cnt, 0, sizeof(cnt));
    while(fgets(s, sizeof(s), f)){
        int len = strlen(s);
        for(int i = 0; i < len; i++)
            cnt[(int)s[i]]++;
    }
    fclose(f);
    bool re = cnt['['] != cnt[']'] || cnt['('] != cnt[')'] || cnt['{'] != cnt['}'];
    if(!silent){
        if(re){
            printf("\033[1;31m");
            printf("balance-braces.................................FAILED\n");
            printf("\033[1;0m");
        }
        else{
            printf("\033[1;32m");
            printf("balance-braces.................................PASSED\n");
            printf("\033[1;0m");
        }
    }
    return re;
}

int run_compile_hook(const char *file, const char *format, bool silent){
    if(strcmp(format, ".cpp") && strcmp(format, ".c")){
        if(!silent){
            printf("\033[1;33m");
            printf("static-error-check..............................SKIPPED\n");
            printf("\033[1;0m");
        }
        return 0;
    }
    copy_file(file, "a.cpp");
    bool re = system("gcc a.cpp -o c.out 2> comp_error_tmp.txt");
    remove("comp_error_tmp.txt");
    remove("a.out");
    remove("a.cpp");
    if(!silent){
        if(re){
            printf("\033[1;31m");
            printf("static-error-check..............................FAILED\n");
            printf("\033[1;0m");
        }
        else{
            printf("\033[1;32m");
            printf("static-error-check..............................PASSED\n");
            printf("\033[1;0m");
        }
    }
    return re;
}

int run_size_hook(const char *file, const char *format, bool silent){
    struct stat st;
    stat(file, &st);
    int size = st.st_size;
    bool re = size > 5000000;
    if(!silent){
        if(re){
            printf("\033[1;31m");
            printf("file-size-check................................FAILED\n");
            printf("\033[1;0m");
        }
        else{
            printf("\033[1;32m");
            printf("file-size-check................................PASSED\n");
            printf("\033[1;0m");
        }
    }
    return re;
}

int run_char_limit_hook(const char *file, const char *format, bool silent){
    if(strcmp(format, ".cpp") && strcmp(format, ".c") && strcmp(format, ".txt")){
        if(!silent){
            printf("\033[1;33m");
            printf("character-limit................................SKIPPED\n");
            printf("\033[1;0m");
        }
        return 0;
    }
    FILE *f = fopen(file, "r");
    char s[MAX_LINE_SIZE];
    int cnt = 0;
    while(fgets(s, sizeof(s), f))
        cnt += strlen(s);
    fclose(f);
    bool re = cnt > 20000;
    if(!silent){
        if(re){
            printf("\033[1;31m");
            printf("character-limit................................FAILED\n");
            printf("\033[1;0m");
        }
        else{
            printf("\033[1;32m");
            printf("character-limit................................PASSED\n");
            printf("\033[1;0m");
        }
    }
    return re;
}

