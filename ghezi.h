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

static const int MAX_CONFIG_NAME = 210;
static const int MAX_PATH_SIZE   = 510;

int remove_prefix(char *av, char *pat);
void invalid_command();
int chdir_ghezi();

int run_init(int argc, char * const argv[]);

int update_config(char *name, char *email);
int update_config_single(char *name, int ty);
int update_general_config(char *infor, int ty);
int reset_general_config();