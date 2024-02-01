#include "ghezi.h"

int commit(const char *msg){
    if(chdir_ghezi())
        return 1;
    if(is_file_empty(stage_name))
        return fprintf(stderr, "no file is staged for commit!"), 0;
    char *commit_id = creat_new_commit(msg);
    char path[2048];
    if(getcwd(path, sizeof(path)) == NULL)
        return 1;
    add_to_string(path, "/commits/", commit_id);
    char commit_files[2048], commit_stage_keeper[2048];
    strcpy(commit_files, path);
    strcpy(commit_stage_keeper, path);
    add_to_string(commit_files, "/", commit_paths);
    add_to_string(commit_stage_keeper, "/", stage_name);
    copy_file(stage_name, commit_files);
    copy_file(stage_name, commit_stage_keeper);
    FILE *head = fopen(head_name, "r");
    char rl[1024], cp[1024];
    while(fscanf(head, "%s %s\n", rl, cp) > 0) if(!is_in_file(commit_files, rl)){
        FILE *f = fopen(commit_files, "a");
        fprintf(f, "%s %s\n", rl, cp);
        fclose(f);
    }
    fclose(head);
    FILE *f = fopen(stage_name, "w");
    fclose(f);
    return copy_file(commit_files, head_name);
}

char* creat_new_commit(const char *msg){
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        fprintf(stderr, "runtime error in function creat_new_commit\n");
    if(chdir_ghezi())
        fprintf(stderr, "runtime error in function creat_new_commit\n");

    FILE *cur_branch = fopen(branch_name, "r");
    if(cur_branch == NULL)
        fprintf(stderr, "can't open file %s in function creat_new_commit\n", branch_name);
    char branch[1024];
    fscanf(cur_branch, "%s", branch);
    fclose(cur_branch);

    FILE *config = fopen("config.txt", "r");
    char infor[MAX_LINE_SIZE];
    if(fgets(infor, sizeof(infor), config) == NULL)
        fprintf(stderr, "runtime error in function creat_new_commit\n");
    fclose(config);
    char *id = get_inc(new_commit_id_keeper);
    if(chdir("commits"))
        fprintf(stderr, "runtime error in function creat_new_commit\n");
    if(mkdir(id, 0755))
        fprintf(stderr, "runtime error in function creat_new_commit\n");
    if(chdir(id))
        fprintf(stderr, "runtime error in function creat_new_commit\n");

    // user
    FILE *f = fopen(commit_user, "w");
    fprintf(f, "%s", infor);
    fclose(f);

    // time
    time_t rawtime;
    struct tm * cur_tm;
    time(&rawtime);
    cur_tm = localtime(&rawtime);
    f = fopen(commit_time, "w");
    fprintf(f, "%d %d %d %d %d %d", cur_tm->tm_year, cur_tm->tm_mon, cur_tm->tm_mday, cur_tm->tm_hour, cur_tm->tm_min, cur_tm->tm_sec);
    fclose(f);

    // message
    f = fopen(commit_message, "w");
    fprintf(f, "%s", msg);
    fclose(f);

    // branch informations
    f = fopen(commit_branch, "w");
    fprintf(f, "%s", branch);
    fclose(f);

    printf ("Commit \"%s\" recorded with id %s in %s", msg, id, asctime(cur_tm));

    if(chdir(cwd))
        fprintf(stderr, "runtime error in function creat_new_commit\n");
    return id;
}

int set_message_shortcut(const char *msg, const char *short_cut){
    if(chdir_ghezi())
        return 1;
    FILE *f = fopen(msg_shortcuts, "a");
    fprintf(f, "%s %s\n", short_cut, msg);
    fclose(f);
    return 0;
}

int remove_message_shortcut(const char *short_cut){
    if(chdir_ghezi())
        return 1;
    FILE *f = fopen(msg_shortcuts, "r");
    FILE *f2 = fopen("tmp.txt", "w");
    char s[MAX_LINE_SIZE];
    bool found = false;
    while(fgets(s, sizeof(s), f)){
        if(!remove_prefix(s, short_cut))
            found = true;
        else
            fprintf(f2, "%s", s);
    }
    fclose(f);
    fclose(f2);
    if(!found){
        remove("tmp.txt");
        return fprintf(stderr, "shortcut %s does not exist!\n", short_cut), 0;
    }
    remove(msg_shortcuts);
    return rename_file("tmp.txt", msg_shortcuts);
}

int replace_message_shortcut(const char *msg, const char *short_cut){
    if(chdir_ghezi())
        return 1;
    if(!is_in_file(msg_shortcuts, short_cut))
        return fprintf(stderr, "shortcut %s does not exist!\n", short_cut), 0;
    if(remove_message_shortcut(short_cut))
        return 1;
    return set_message_shortcut(msg, short_cut);
}

char *find_short_cut(const char *short_cut){
    if(chdir_ghezi())
        fprintf(stderr, "runtime error in function find_short_cut\n");
    FILE *f = fopen(msg_shortcuts, "r");
    char s[MAX_LINE_SIZE];
    while(fgets(s, sizeof(s), f)) if(!remove_prefix(s, short_cut)){
        char *msg = malloc(MAX_COMMIT_MESSAGE_SIZE + 100);
        strcpy(msg, s + 1);
        msg[strlen(msg) - 1] = '\0';
        return msg;
    }
    char *x = malloc(10);
    x[0] = '\0';
    return x;
}