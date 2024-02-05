#include "ghezi.h"

int max(int a, int b){
    return a < b ? b : a;
}

int min(int a, int b){
    return a < b ? a : b;
}

void invalid_command(){
    if(!get_silent())
        fprintf(stderr, "please enter a valid commnad!\n");
}

void runtime_in_function(char *function_name){
    fprintf(stderr, "runtime in function %s\n", function_name);
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

bool is_allowed_name(const char *s){
    return strcmp(s, ".") && strcmp(s, "..") && strcmp(s, ".ghezi");
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

int remove_prefix(char *av, const char *pat){
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
    if(!strcmp(path1, path2))
        return 0;
    if(!strcmp(path1, "NULL") || !strcmp(path2, "NULL"))
        return 1;
    FILE *f1 = fopen(path1, "r");
    FILE *f2 = fopen(path2, "r");
    if(f1 == NULL && f2 == NULL)
        return 0;
    if(f1 == NULL || f2 == NULL){
        if(f1 != NULL)
            fclose(f1);
        if(f2 != NULL)
            fclose(f2);
        return 1;
    }

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

bool is_file_here(const char *path, const char *file){
    int len = strlen(path);
    if(strlen(path) < len)
        return 0;
    for(int i = 0; i < len; i++)
        if(path[i] != file[i])
            return 0;
    if(file[len] != '/')
        return 0;
    int len2 = strlen(file);
    for(int i = len + 1; i < len2; i++)
        if(file[i] == '/')
            return 0;
    return 1;
}

bool is_in_file(const char *path, const char *pat){
    FILE *f = fopen(path, "r");
    if(f == NULL)
        return false;
    char s[1024];
    bool re = false;
    while(fscanf(f, "%s \n", s) > 0)
        re |= (!strcmp(pat, s));
    fclose(f);
    return re;
}

char* find_in_map(const char *path, const char *pat){
    char *s = malloc(1024);
    char x[1024], y[1024];
    s[0] = '\0';
    FILE *f = fopen(path, "r");
    while(fscanf(f, "%s %s\n", x, y) > 0)
        if(!strcmp(pat, x))
            strcpy(s, y);
    fclose(f);
    return s;
}

bool is_file_empty(const char *path){
    FILE *f = fopen(path, "r");
    char s[MAX_LINE_SIZE];
    bool re = fgets(s, sizeof(s), f) == NULL;
    fclose(f);
    return re;
}

char* get_inc(const char *path){
    char *new_name = malloc(MAX_LINE_SIZE);
    new_name[0] = '\0';
    FILE *nname = fopen(path, "r");
    if(nname == NULL)
        return fprintf(stderr, "error while opening file %s in function get_inc\n", path), new_name;
    fscanf(nname, "%s", new_name);
    fclose(nname);
    if((nname = fopen(path, "r")) == NULL)
        return fprintf(stderr, "error while opening file %s in function get_inc\n", path), new_name;
    int x;
    fscanf(nname, "%d", &x);
    fclose(nname);
    if((nname = fopen(path, "w")) == NULL)
        return fprintf(stderr, "error while opening file %s in function get_inc\n", path), new_name;
    fprintf(nname, "%d", ++x);
    fclose(nname);
    return new_name;
}

char *get_stage_path(){
    char *path = get_ghezi_wd();
    add_to_string(path, "/", stage_name);
    return path;
}

char *get_track_path(){
    char *path = get_ghezi_wd();
    add_to_string(path, "/", tracker_name);
    return path;
}

char *get_head_path(){
    char *path = get_ghezi_wd();
    add_to_string(path, "/", head_name);
    return path;
}

int add_to_begining(const char *path, const char *s){
    FILE *f = fopen(path, "r");
    if(f == NULL)
        fprintf(stderr, "error while opening file %s in function add_to_begining\n", path);
    FILE *t = fopen("add_to_begining_tmp.txt", "w");
    fprintf(t, "%s\n", s);
    char tmp[MAX_LINE_SIZE];
    while(fgets(tmp, sizeof(tmp), f))
        fprintf(t, "%s", tmp);
    fclose(f);
    fclose(t);
    if(system(string_concat2("rm ", abs_path(path))))
        return 1;
    return rename_file("add_to_begining_tmp.txt", path);
}

bool is_less_equal(struct tm a, struct tm b){
    if(a.tm_year < b.tm_year)
        return true;
    if(a.tm_year > b.tm_year)
        return false;
    if(a.tm_mon < b.tm_mon)
        return true;
    if(a.tm_mon > b.tm_mon)
        return false;
    if(a.tm_mday < b.tm_mday)
        return true;
    if(a.tm_mday > b.tm_mday)
        return false;
    if(a.tm_hour < b.tm_hour)
        return true;
    if(a.tm_hour > b.tm_hour)
        return false;
    if(a.tm_min < b.tm_min)
        return true;
    if(a.tm_min > b.tm_min)
        return false;
    if(a.tm_sec > b.tm_sec)
        return false;
    return true;
}

char* string_concat2(const char *s1, const char *s2){
    char *s = malloc(strlen(s1) + strlen(s2) + 100);
    strcpy(s, s1);
    strcpy(s + strlen(s), s2);
    return s;
}

char* string_concat(const char *s1, const char *s2, const char *s3){
    return string_concat2(string_concat2(s1, s2), s3);
}

int get_num(const char *dig){
    int x = 0, len = strlen(dig);
    for(int i = 0; i < len; i++){
        if(dig[i] > '9' || dig[i] < '0')
            return -1;
        x *= 10;
        x += dig[i] - '0';
    }
    return x;
}

struct tm make_tm_from_date(const char *cdate){
    char *date = malloc(1024);
    strcpy(date, cdate);
    struct tm cur;
    cur.tm_hour = cur.tm_min = cur.tm_sec = 0;
    int id = 0;
    while(date[id] != '/')
        id++;
    date[id] = '\0';
    cur.tm_year = get_num(date) - 1900;
    int keep = id;
    while(date[id] != '/')
        id++;
    date[id] = '\0';
    cur.tm_mon = get_num(date + keep + 1);
    cur.tm_mday = get_num(date + id + 1);
    return cur;
}

char* get_commit_path(char *commit_id){
    char *path = get_ghezi_wd();
    add_to_string(path, "/commits/", commit_id);
    return path;
}

bool is_white_space(char c){
    return c == ' ' || c == '\n' || c == '\0' || c == '\t';
}

int remove_all_here(){
    struct dirent *entry;
    DIR *dir = opendir(".");
    if(dir == NULL)
        return 1;
    while((entry = readdir(dir)) != NULL) if(is_allowed_name(entry->d_name))
        if(system(string_concat2("rm -rf ", string_concat("\"", entry->d_name, "\""))))
            return 1;
    closedir(dir);
    return 0;
}

bool is_deleted(char *path){
    FILE *f = fopen(path, "r");
    if(f == NULL)
        return true;
    fclose(f);
    return false;
}

int make_file(const char *cppath){
    char *path = malloc(1024);
    strcpy(path, cppath);
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;
    
    if(chdir("/"))
        return 1;
    int last = 1, len = strlen(path), i = 1;
    while(i < len){
        if(path[i] == '/'){
            char name[1024];
            int pt = 0;
            for(int j = last; j < i; j++)
                name[pt++] = path[j];
            name[pt] = '\0';
            if(chdir(name)){
                if(mkdir(name, 0755) || chdir(name))
                    return 1;
            }
            last = i + 1;
        }
        i++;
    }
    char file_name[1024];
    int pt = 0;
    for(int i = last; i < len; i++)
        file_name[pt++] = path[i];
    FILE *f = fopen(file_name, "w");
    fclose(f);
    return chdir(cwd) != 0;
}

bool is_in_head(){ // error -> return 0
    char cwd[1024];
    if(getcwd(cwd, sizeof(cwd)) == NULL)
        return runtime_in_function("is_in_head"), 0;
    if(chdir_ghezi())
        return runtime_in_function("is_in_head"), 0;
    char id[1024], br[1024], head[1024];
    FILE *f = fopen(last_commit, "r");
    if(f == NULL)
        return runtime_in_function("is_in_head"), 0;
    fscanf(f, "%s", id);
    fclose(f);

    f = fopen(branch_name, "r");
    if(f == NULL)
        return runtime_in_function("is_in_head"), 0;
    fscanf(f, "%s", br);
    fclose(f);


    if(chdir("branch"))
        return runtime_in_function("is_in_head"), 0;
    
    f = fopen(br, "r");
    if(f == NULL)
        return runtime_in_function("is_in_head"), 0;
    fscanf(f, "%s \n", head);
    fclose(f);
    bool re = !strcmp(head, id);

    if(chdir(cwd))
        return runtime_in_function("is_in_head"), 0;
    return re;
}

bool is_commit_silented(const char *id){
    char *dir = get_ghezi_wd();
    add_to_string(dir, "/commits/", id);
    dir = string_concat(dir, "/", commit_silented);
    FILE *f = fopen(dir, "r");
    if(f == NULL)
        return false;
    fclose(f);
    return true;
}

char* find_in_map_with_space(const char *file_path, const char *pat){
    FILE *f = fopen(file_path, "r");
    char *emp = malloc(10);
    emp[0] = '\0';
    if(f == NULL)
        return emp;
    char s[MAX_LINE_SIZE];
    while(fgets(s, sizeof(s), f)) if(!remove_prefix(s, pat)){
        char *msg = malloc(MAX_COMMIT_MESSAGE_SIZE);
        strcpy(msg, s + 1);
        msg[strlen(msg) - 1] = '\0';
        return msg;
    }
    fclose(f);
    return emp;
}

bool is_valid_command(const char *command){
    return !system(string_concat2("ghezi -SILENT ", command));
}

bool is_commit_id_valid(const char *id){
    char *dir = get_ghezi_wd();
    add_to_string(dir, "/", all_commits);
    return is_in_file(dir, id);
}

char* string_concat_master(int n, char **strings, int id, char *ext){
    int pt = 0;
    char *res = malloc(1024);
    for(int i = 0; i < n; i++){
        if(i == id){
            int len = strlen(ext);
            for(int j = 0; j < len; j++)
                res[pt++] = ext[j];
            res[pt++] = ' ';
        }
        int len = strlen(strings[i]);
        for(int j = 0; j < len; j++)
            res[pt++] = strings[i][j];
        res[pt++] = ' ';
    }
    res[pt - 1] = '\0';
    return res;
}

bool is_wildcard(const char *s){
    int len = strlen(s);
    for(int i = 0; i < len; i++)
        if(s[i] == '*')
            return true;
    return false;
}

bool wildcard_match(const char *av, const char *pat){
    int pt = 0, len_av = strlen(av), len_pat = strlen(pat);
    while(pt < min(len_av, len_pat) && pat[pt] == av[pt])
        pt++;
    if(pt == len_av && pt == len_pat)
        return true;
    if(pat[pt] != '*')
        return false;
    pt = len_pat - 1;
    int pt2 = len_av - 1;
    while(pat[pt] == av[pt2]){
        pt--;
        pt2--;
    }
    return pat[pt] == '*';
}

char* get_head_x_commit(const int step){
    char *dir = get_ghezi_wd();
    FILE *f = fopen(string_concat(dir, "/", branch_name), "r");
    if(f == NULL)
        runtime_in_function("get_head_x_commit");
    char *emp = malloc(10);
    emp[0] = '\0';
    char branch[1024], tmp[1024];
    char *id = malloc(1024);
    fscanf(f, "%s", branch);
    fclose(f);
    add_to_string(dir, "/branch/", branch);
    f = fopen(dir, "r");
    if(f == NULL)
        runtime_in_function("get_head_x_commit");
    int n = step;
    n++;
    while(n--){
        if(fscanf(f, "%s \n", id) <= 0)
            return fprintf(stderr, "less than %d commits exist in this branch!\n", step), emp;
        
    }
    if(fscanf(f, "%s \n", tmp) <= 0)
        return fprintf(stderr, "less than %d commits exist in this branch!\n", step), emp;
    fclose(f);
    return id;
}

struct tm get_current_time(){
    time_t rawtime;
    struct tm * cur_tm;
    time(&rawtime);
    cur_tm = localtime(&rawtime);
    return *cur_tm;
}

int write_time(const char *path, const struct tm tim){
    FILE *f = fopen(path, "w");
    if(f == NULL)
        return 1;
    fprintf(f, "%d %d %d %d %d %d %d", tim.tm_year, tim.tm_mon, tim.tm_mday, tim.tm_wday, tim.tm_hour, tim.tm_min, tim.tm_sec);
    fclose(f);
    return 0;
}

struct tm read_time(const char *path){
    FILE *f = fopen(path, "r");
    struct tm tim;
    tim.tm_hour = 0;
    if(f == NULL)
        return runtime_in_function("read_time"), tim;
    fscanf(f, "%d %d %d %d %d %d %d", &tim.tm_year, &tim.tm_mon, &tim.tm_mday, &tim.tm_wday, &tim.tm_hour, &tim.tm_min, &tim.tm_sec);
    fclose(f);
    return tim;
}

char* get_user_information(){
    char *dir = get_ghezi_wd();
    add_to_string(dir, "/", config_name);
    FILE *config = fopen(dir, "r");
    if(config == NULL)
        runtime_in_function("get_user_information");
    char *infor = malloc(2048);
    if(fgets(infor, 2048, config) == NULL)
        runtime_in_function("get_user_information");
    fclose(config);
    return infor;
}

int remove_from_map(const char *file_path, const char *pat){
    FILE *f = fopen(file_path, "r");
    FILE *t = fopen("remove_from_map_tmp.txt", "w");
    if(f == NULL || t == NULL)
        return 1;
    char rl[1024], cp[1024];
    while(fscanf(f, "%s %s\n", rl, cp) > 0){
        if(strcmp(rl, pat))
            fprintf(t, "%s %s\n", rl, cp);
    }
    fclose(f);
    fclose(t);
    if(copy_file("remove_from_map_tmp.txt", file_path))
        return 1;
    remove("remove_from_map_tmp.txt");
    return 0;
}

char* get_current_commit_id(){
    char *dir = get_ghezi_wd();
    FILE *f = fopen(string_concat(dir, "/", last_commit), "r");
    if(f == NULL)
        runtime_in_function("get_current_commit_id");
    char *s = malloc(1024);
    fscanf(f, "%s", s);
    fclose(f);
    return s;
}

int remove_from_file(const char *file_path, const char *pat){
    FILE *f = fopen(file_path, "r");
    FILE *t = fopen("remove_from_file_tmp.txt", "w");
    char s[1024];
    while(fscanf(f, "%s \n", s) > 0)
        if(strcmp(pat, s))
            fprintf(t, "%s\n", s);
    fclose(f);
    fclose(t);
    if(copy_file("remove_from_file_tmp.txt", file_path))
        return 1;
    remove("remove_from_file_tmp.txt");
    return 0;
}

bool is_null(const char *s){
    int len = strlen(s);
    bool re = true;
    for(int i = 0; i < len; i++)
        re &= is_white_space(s[i]);
    return re;
}

char* get_next_valid_line(FILE *file_pointer, int *cur_line){
    char *s = malloc(MAX_LINE_SIZE);
    s[0] = '\0';
    while(is_null(s)){
        (*cur_line)++;
        if(fgets(s, MAX_LINE_SIZE, file_pointer) == NULL){
            s[0] = '\0';
            return s;
        }
    }
    return s;
}

int find_char_in_string(const char *string, const char c){
    int len = strlen(string);
    for(int i = 0; i < len; i++)
        if(string[i] == c)
            return i;
    return -1;
}

char* get_file_format(const char *file){
    int len = strlen(file);
    char *res = malloc(1024);
    res[0] = '\0';
    for(int i = len - 1; i >= 0; i--)
        if(file[i] == '.'){
            strcpy(res, file + i);
            return res;
        }
    return res;
}