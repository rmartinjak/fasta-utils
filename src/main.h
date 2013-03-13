#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>

#define FASTA_MAINOPTS "a:w:"

#define FASTA_DEFAULTWIDTH 0
extern int main_width;

#define FASTA_NOSTDIN (1 << 0)
#define FASTA_ONEFILE (1 << 1)
extern int config;


/* implemented by main.c */
unsigned fasta_parse_uint(const char *s, const char *errmsg);
int fasta_main_getopt(int opt, char *arg);


/* implemented by the tool */
int fasta_init(void);
int fasta_getopt(int argc, char **argv);

void fasta_file_begin(const char *path, FILE *stream);
void fasta_file_end(void);

int fasta_process_seq(const char *id, const char *comment, const char *seq);
#endif
