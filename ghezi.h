#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

static const char *general_configs_path = "/Users/fatemetmhr/Documents/ghezi";
static const char *general_configs_name = "general_config_paths.txt";
static const char *last_general_name = "general_name.txt";
static const char *last_general_email = "general_email.txt";
static const char *stage_name = "stage.txt";
static const char *new_name_keeper = "source_name_counter.txt";
static const char *tracker_name = "tracker.txt";

static const char *stage_history[11] = {"stage.txt", "stage0.txt", "stage1.txt", "stage2.txt", "stage3.txt", "stage4.txt", "stage5.txt", "stage6.txt", "stage7.txt", "stage8.txt", "stage9.txt"};

static const int MAX_CONFIG_NAME   = 210;
static const int MAX_PATH_SIZE     = 510;
static const int MAX_LINE_SIZE     = 2048;
static const int MAX_STAGE_HISTORY = 11;

// general functions

int remove_prefix(char *av, char *pat);
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

// init

int run_init(int argc, char * const argv[]);

// config

int update_config(char *name, char *email);
int update_config_single(char *name, int ty);
int update_general_config(char *infor, int ty);
int reset_general_config();

// add

int add_file(char *name);
int add_dir();
int show_stage_status();
int reset_file(char *name);
int reset_dir();
int shift_stage_history(int stp);

// status

int status();