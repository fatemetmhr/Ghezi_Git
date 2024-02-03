#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

static const char *general_configs_path = "/Users/fatemetmhr/Documents/ghezi";
static const char *general_configs_name = "general_config_paths.txt";
static const char *general_alias = "general_alias.txt";
static const char *last_general_name = "general_name.txt";
static const char *last_general_email = "general_email.txt";
static const char *stage_name = "stage.txt";
static const char *branch_name = "cur_branch.txt";
static const char *new_name_keeper = "source_name_counter.txt";
static const char *new_commit_id_keeper = "commit_id_counter.txt";
static const char *tracker_name = "tracker.txt";
static const char *commit_paths = "commits.txt";
static const char *commit_time = "time.txt";
static const char *commit_user = "information.txt";
static const char *commit_message = "message.txt";
static const char *commit_branch = "branch_name.txt";
static const char *commit_silented = "is_silented.txt";
static const char *head_name = "HEAD_commit.txt";
static const char *msg_shortcuts = "commit_message_shortcuts.txt";
static const char *all_commits = "all_commits_ids.txt";
static const char *last_commit = "last_commit_id.txt";
static const char *alias_name = "alias.txt";
static const char *tag_name = "tag.txt";
static const char *commits_with_tag_name = "commits_with_tag.txt";
static const char *tag_commit = "tag.txt";
static const char *tag_user = "user.txt";
static const char *tag_date = "date.txt";
static const char *tag_message = "message.txt";
static const char *config_name = "config.txt";


static const char *stage_history[11] = {"stage.txt", "stage0.txt", "stage1.txt", "stage2.txt", "stage3.txt", "stage4.txt", "stage5.txt", "stage6.txt", "stage7.txt", "stage8.txt", "stage9.txt"};

static const int MAX_CONFIG_NAME         = 210;
static const int MAX_PATH_SIZE           = 510;
static const int MAX_LINE_SIZE           = 2048;
static const int MAX_STAGE_HISTORY       = 11;
static const int MAX_COMMIT_MESSAGE_SIZE = 72;


// general functions

int remove_prefix(char *av, const char *pat);
void invalid_command();
int chdir_ghezi();
void add_to_string(char *src, const char *add1, const char *add2);
int copy_file(const char *src, const char *dst);
int are_diff(const char *path1, const char *path2);
char* make_par_dir(char *path);
int max(int a, int b);
int min(int a, int b);
int rename_file(const char *cur_name, const char *wanted_name);
void debug(const char *s);
void print_cwd();
char* get_ghezi_wd();
bool is_file_here(const char *path, const char *file);
bool is_in_file(const char *path, const char *pat);
char* find_in_map(const char *path, const char *pat);
bool is_allowed_name(const char *s);
bool is_file_empty(const char *path);
char* get_inc(const char *path);
char* get_stage_path();
char* get_track_path();
char* get_head_path();
char* get_commit_path(char *commit_id);
int add_to_begining(const char *path, const char *s);
bool is_less_equal(struct tm a, struct tm b);
char* string_concat2(const char *s1, const char *s2);
char* string_concat(const char *s1, const char *s2, const char *s3);
struct tm make_tm_from_date(const char *date);
int get_num(const char *dig);
bool is_white_space(char c);
int remove_all_here();
bool is_deleted(char *file_path);
void runtime_in_function(char *function_name);
char* abs_path(const char *path);
int make_file(const char *file_abs_path);
bool is_in_head();
bool is_commit_silented(const char *commit_id);
char* find_in_map_with_space(const char *file_path, const char *pat);
bool is_valid_command(const char *command);
bool get_silent();
bool is_commit_id_valid(const char *commit_id);
char* string_concat_master(int n, char **strings, int id, char *extra_string);
bool is_wildcard(const char *s);
bool wildcard_match(const char *have, const char *wildcard);
char* get_head_x_commit(const int step);
struct tm get_current_time();
int write_time(const char *file_path, const struct tm _time);
struct tm read_time(const char *file_path);
char* get_user_information();
int remove_from_map(const char *file_path, const char *pat);
char* get_current_commit_id();
int remove_from_file(const char *file_path, const char *pat);


// init

int run_init(int argc, char * const argv[]);

// config

int update_config(char *name, char *email);
int update_config_single(char *name, int ty);
int update_general_config(char *infor, int ty);
int reset_general_config();
int set_alias(char *alias, char *command);
int set_general_alias(char *alias, char *command);

// add

int add_file(const char *name);
int add_dir();
int show_stage_status(const char *cur_path);
int show_stage_status_recursive(const char *cur_name, int depth);
int reset_file(const char *name);
int reset_dir();
int shift_stage_history(int stp);
int redo_add();

// status

bool status(bool silent);
bool find_untracking_files(bool silent);

// commit

int commit(const char *message, bool force_commit, bool silent_commit);
char* creat_new_commit(const char *message, bool silent);
int set_message_shortcut(const char *message, const char *short_cut);
int remove_message_shortcut(const char *short_cut);
int replace_message_shortcut(const char *message, const char *short_cut);
char* find_short_cut(const char *short_cut);
int print_commit_informations(const char *commit_id);

// branch

int make_branch(const char *name);
int show_all_branchs();

// log

int show_all_logs(int num);
int show_all_branch_commits(char *branch_name);
int show_all_personal_commits(char *person_name);
int show_all_word_match_commits(int num, char **words);
int show_all_during_commits(struct tm _from, struct tm _before);

// checkout

int checkout_to_commit(const char *commit_id);
int checkout_to_branch(const char *branch_name);
int checkout_to_head(int before_head);

// revert

int revert_to_commit(const char *commit_id, const char *message, bool do_commit);
int revert_to_head(int step, const char *message);

// tag

int add_tag(const char *tag, const char *commit_id, const char *message, bool overwrite);
int show_all_tags();
int print_tag(const char *tag);