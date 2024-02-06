#include "ghezi.h"

int add_tag(const char *tag, const char *commit_id, const char *message, bool overwrite){
    if(get_silent())
        return 0;
    if(chdir_ghezi())
        return 1;
    bool in_file = is_in_file(tag_name, tag);
    if(in_file && !overwrite)
        return fprintf(stderr, "tag %s already exist. use flag -f to overwrite\n", tag), 0;
    if(!in_file){
        FILE *f = fopen(tag_name, "a");
        fprintf(f, "%s\n", tag);
        fclose(f);
    }
    else
        system(string_concat2("rm -rf tag/", tag));
    if(chdir("tag"))
        return 1;
    if(mkdir(tag, 0755))
        return 1;
    if(chdir(tag))
        return 1;
    
    // commit id
    FILE *f = fopen(tag_commit, "w");
    fprintf(f, "%s", commit_id);
    fclose(f);

    // user
    f = fopen(tag_user, "w");
    fprintf(f, "%s", get_user_information());
    fclose(f);

    // date
    if(write_time(tag_date, get_current_time()))
        return 1;

    // message
    f = fopen(tag_message, "w");
    fprintf(f, "%s", message);
    fclose(f);

    return 0;
}

int show_all_tags(){
    if(get_silent())
        return 0;
    if(chdir_ghezi())
        return 1;
    FILE *f = fopen(tag_name, "r");
    char tag[1024];
    while(fscanf(f, "%s \n", tag) > 0)
        printf("%s\n", tag);
    fclose(f);
    return 0;
}

int print_tag(const char *tag){
    if(get_silent())
        return 0;
    if(chdir_ghezi() || chdir("tag"))
        return 1;
    if(chdir(tag))
        return fprintf(stderr, "no such tag exist!\n"), 0;
    printf("Tag %s:\n", tag);
    char s[1024], t[1024];

    // commit id
    FILE *f = fopen(tag_commit, "r");
    if(f == NULL)
        return 1;
    fscanf(f, "%s", s);
    fclose(f);
    printf("commit %s\n", s);

    // user
    f = fopen(tag_user, "r");
    if(f == NULL)
        return 1;
    fscanf(f, "%s %s", s, t);
    fclose(f);
    printf("Author: %s <%s>\n", s, t);

    // date
    struct tm cur = read_time(tag_date);
    printf("Date: %s", asctime(&cur));

    // message
    f = fopen(tag_message, "r");
    if(f == NULL)
        return 1;
    fgets(s, sizeof(s), f);
    fclose(f);
    printf("Message: %s\n", s);

    return 0;
}