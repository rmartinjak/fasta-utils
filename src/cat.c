#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

int main_config = 0;


int
tool_init(void)
{
    return FASTA_OK;
}

void
tool_destroy(void)
{
}

int
tool_getopt(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, MAIN_OPTS)) != -1) {
        switch (opt) {
            case '?':
                exit(EXIT_FAILURE);
            default:
                main_getopt(opt, optarg);
        }
    }
    return optind;
}

void
tool_file_begin(const char *path, FILE *stream)
{
    (void) path;
    (void) stream;
}

void
tool_file_end(void)
{
}

int
tool_process_seq(const char *id, const char *comment, const char *seq)
{
    fasta_write(stdout, id, comment, seq, main_width);
    return FASTA_OK;
}
