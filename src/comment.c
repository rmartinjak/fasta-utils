#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "fasta.h"
#include "main.h"

int main_config = 0;


static int append = 0;
static char *new_comment = NULL;

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
    while ((opt = getopt(argc, argv, MAIN_OPTS "c:k")) != -1) {
        switch (opt) {
            case 'k':
                append = 1;
                break;
            case 'c':
                new_comment = optarg;
                break;
            case '?':
                exit(EXIT_FAILURE);
            default:
                main_getopt(opt, optarg);
        }
    }
    return optind;
}

void
tool_file_begin(const char *path, gzFile stream)
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
    char *c = NULL;
    size_t sz = 1;
    if (!append || !(comment && *comment)) {
        comment = NULL;
    }

    if (comment) {
        sz += strlen(comment);
    }
    if (new_comment) {
        sz += strlen(new_comment);
    }

    if (!(c = malloc(sz))) {
        return FASTA_ERROR;
    }
    *c = '\0';
    if (comment) {
        strcat(c, comment);
    }
    if (comment && new_comment) {
        strcat(c, "\n");
    }
    if (new_comment) {
        strcat(c, new_comment);
    }
    fasta_write(stdout, id, c, seq, main_width);
    free(c);
    return FASTA_OK;
}
