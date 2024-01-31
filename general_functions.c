#include "ghezi.h"

int max(int a, int b){
    return a < b ? b : a;
}

int min(int a, int b){
    return a < b ? a : b;
}

void invalid_command(){
    fprintf(stderr, "please enter a valid commnad!");
}

void debug(const char *s){
    fprintf(stderr, "%s\n", s);
}

void print_cwd(){
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        fprintf(stderr, "error while getting current path, in print_cwd function");
    debug(cwd);
    return;
}

char* get_ghezi_wd(){
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        fprintf(stderr, "error while getting current path, in get_ghezi_wd function");
    char *path = malloc(2048);
    chdir_ghezi();
    if(getcwd(path, 2048) == NULL)
        fprintf(stderr, "error while getting current path, in get_ghezi_wd function");
    chdir(cwd);
    return path;
}

char* abs_path(const char *s){
    if(s[0] == '/'){
        char *t = malloc(1024);
        strcpy(t, s);
        return t;
    }
    char *cwd = malloc(1024);
    if(getcwd(cwd, 1024) == NULL)
        fprintf(stderr, "error while getting current path, in abs_path function");
    add_to_string(cwd, "/", s);
    return cwd;
}

int copy_file(const char *src, const char *dst){
    char *s = malloc(1024);
    s[0] = 'c';
    s[1] = 'p';
    s[2] = ' ';
    strcpy(s + 3, abs_path(src));
    int len = strlen(s);
    s[len] = ' ';
    strcpy(s + len + 1, abs_path(dst));
    return system(s);
}

int rename_file(const char *s, const char *t){
    copy_file(s, t);
    remove(s);
    return 0;
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

int chdir_ghezi(){ // ghezi found -> return 0 else if no ghezi found -> 1 else if any error -> return 2;
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

void add_to_string(char *s, const char *a, const char *b){
    int lens = strlen(s);
    strcpy(s + lens, a);
    lens = strlen(s);
    strcpy(s + lens, b);
}

int are_diff(const char *path1, const char *path2){ // any error or are diff -> return 1
    FILE *f1 = fopen(path1, "r");
    FILE *f2 = fopen(path2, "r");
    if(f1 == NULL || f2 == NULL)
        return fprintf(stderr, "error while opening files %s %s\n", path1, path2), 1;

    char s[MAX_LINE_SIZE], t[MAX_LINE_SIZE];
    int re = 0;
    char *eof1, *eof2;
    //*
    while(true){
        eof1 = fgets(s, sizeof(s), f1); 
        eof2 = fgets(t, sizeof(t), f2);
        if(eof1 == NULL && eof2 == NULL)
            break;
        if(eof1 == NULL || eof2 == NULL){
            re = 1;
            break;
        }
        if(strcmp(s, t)){
            re = 1;
            break;
        }
    }
    //*/
    fclose(f1);
    fclose(f2);
    return re;
}

char* make_par_dir(char *path){
    int len = strlen(path);
    char *name = malloc(1024);
    int pt = len - 1;
    while(pt >= 0 && path[pt] != '/')
        pt--;
    strcpy(name, path + pt + 1);
    pt = max(pt, 0);
    path[pt] = '\0';
    return name;
}