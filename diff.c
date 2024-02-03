#include "ghezi.h"

bool file_diff_checker(const char *file1, const char *file2, int begin1, int end1, int begin2, int end2, bool silent, bool show_extra_informatin, const char *ext1, const char *ext2){
    if(get_silent())
        return 0;
    char *path1 = abs_path(file1);
    char *path2 = abs_path(file2);
    char *cp = malloc(1024);
    strcpy(cp, file1);
    char *name1 = make_par_dir(cp);
    strcpy(cp, file2);
    char *name2 = make_par_dir(cp);

    bool diff = false;

    FILE *f1 = fopen(path1, "r");
    FILE *f2 = fopen(path2, "r");

    if(f1 == NULL || f2 == NULL)
        return fprintf(stderr, "files should exist in order to check diff!"), 1;

    char s[MAX_LINE_SIZE];

    int line1 = 0, line2 = 0;
    while(line1 + 1 < begin1){
        line1++;
        fgets(s, sizeof(s), f1);
    }
    while(line2 + 1 < begin2){
        line2++;
        fgets(s, sizeof(s), f2);
    }

    while(line1 <= end1 || line2 <= end2){
        char *s1 = get_next_valid_line(f1, &line1);
        if(line1 > end1)
            s1[0] = '\0';
        char *s2 = get_next_valid_line(f2, &line2);
        if(line2 > end2)
            s2[0] = '\0';
        if(!strlen(s1) && !strlen(s2))
            break;
        if(strcmp(s1, s2)){
            diff = true;
            if(silent)
                continue;
            printf("«««««\n");
            printf("%s - %d\n", show_extra_informatin ? ext1 : name1, line1);
            printf("%s\n", s1);
            printf("%s - %d\n", show_extra_informatin ? ext2 : name2, line2);
            printf("%s\n", s2);
            printf("»»»»»\n");
        }
    }

    fclose(f1);
    fclose(f2);

    return diff;
}

bool commit_diff_checker(const char *id1, const char *id2, bool silent_in_file_diff, bool silent_extra_file, bool show_extra_informatin, const char *ext1, const char *ext2){
    if(get_silent())
        return 0;
    if(chdir_ghezi() || chdir("commits"))
        return runtime_in_function("commit_diff_checker"), 1;
    FILE *av1 = fopen(string_concat(id1, "/", commit_paths), "r");

    if(av1 == NULL)
        return runtime_in_function("commit_diff_checker"), 1;

    bool diff = false;

    char rl[1024], cp[1024];

    while(fscanf(av1, "%s %s\n", rl, cp) > 0){
        char *cp2 = find_in_map(string_concat(id2, "/", commit_paths), rl);
        bool iscp_removed = !strcmp(cp, "NULL");
        bool iscp2_removed = !strcmp(cp2, "NULL");
        if(iscp2_removed && iscp_removed)
            continue;
        if(iscp_removed && !iscp2_removed){
            if(silent_extra_file)
                continue;
            printf("«««««\n");
            printf("file %s exist in commit %s but not in commit %s\n", rl, id2, id1);
            printf("»»»»»\n");
            continue;
        }
        if(!iscp_removed && iscp2_removed){
            if(silent_extra_file)
                continue;
            printf("«««««\n");
            printf("file %s exist in commit %s but not in commit %s\n", rl, id1, id2);
            printf("»»»»»\n");
            continue;
        }
        if(strlen(cp2))
            diff |= file_diff_checker(cp, cp2, 1, MAX_FILE_SIZE, 1, MAX_FILE_SIZE, false, true, string_concat2(ext1, string_concat(rl, " from commit ", id1)), string_concat2(ext2, string_concat(rl, " from commit ", id2)));
        else{
            if(silent_extra_file)
                continue;
            printf("«««««\n");
            printf("file %s exist in commit %s but not in commit %s\n", rl, id1, id2);
            printf("»»»»»\n");
        }
    }
    
    fclose(av1);

    if(silent_extra_file)
        return diff;

    FILE *av2 = fopen(string_concat(id2, "/", commit_paths), "r");
    while(fscanf(av2, "%s %s\n", rl, cp) > 0){
        char *cp2 = find_in_map(string_concat(id1, "/", commit_paths), rl);
        if(!strlen(cp2)){
            if(silent_extra_file)
                continue;
            printf("«««««\n");
            printf("file %s exist in commit %s but not in commit %s\n", rl, id2, id1);
            printf("»»»»»\n");
        }
    }

    return diff;
}