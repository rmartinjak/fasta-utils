#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>

#define MAIN_OPTS "a:w:"

#define MAIN_DEFAULT_WIDTH 0
extern unsigned main_width;

#define MAIN_NO_STDIN (1 << 0)
#define MAIN_ONE_FILE (1 << 1)
extern int main_config;


/* implemented by main.c */
unsigned main_parse_uint(const char *s, const char *errmsg);
int main_getopt(int opt, char *arg);


/* implemented by the tool */
int tool_init(void);
void tool_destroy(void);
int tool_getopt(int argc, char **argv);

void tool_file_begin(const char *path, FILE *stream);
void tool_file_end(void);

int tool_process_seq(const char *id, const char *comment, const char *seq);
#endif
