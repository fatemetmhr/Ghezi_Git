#include "ghezi.h"

int grep(const char *file, const char *word, bool print_line){
    if(get_silent())
        return 0;
    FILE *f = fopen(file, "r");
    if(f == NULL)
        return 1;
    char s[MAX_LINE_SIZE], buf[MAX_LINE_SIZE], keep[MAX_LINE_SIZE];
    char *sep = " \n\t";
    int line = 1;
    while(fgets(s, sizeof(s), f)){
        strcpy(keep, s);
        int x = 0;
        char *very_last = s + strlen(s);
        char *ptr = strtok(s, sep);
        bool re = false;
        buf[0] = '\0';
        char *last = s;
        while(ptr != NULL){
            while(last != ptr){
                int len = strlen(buf);
                buf[len] = keep[x];
                buf[len + 1] = '\0';
                last++;
                x++;
            }
            if(wildcard_match(ptr, word)){
                strcpy(buf + strlen(buf), "\033[1;32m");
                strcpy(buf + strlen(buf), ptr);
                strcpy(buf + strlen(buf), "\033[1;0m");
                re = true;
            }
            else
                strcpy(buf + strlen(buf), ptr);
            while(last != ptr + strlen(ptr)){
                last++;
                x++;
            }
            ptr = strtok(NULL, sep);
        }
        while(last != very_last){
            int len = strlen(buf);
            buf[len] = keep[x];
            buf[len + 1] = '\0';
            last++;
            x++;
        }
        if(re){
            if(print_line)
                printf("%d. ", line);
            printf("%s", buf);
        }
        line++;
    }
    fclose(f);
    return 0;
}