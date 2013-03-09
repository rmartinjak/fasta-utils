#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

int config = 0;

static int brief = 0;
static size_t count, min, max;

#define STRFMT "%-*.*s"
#define STRWIDTH 30, 30


int fasta_init(void)
{
    return FASTA_OK;
}

int fasta_getopt(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, FASTA_MAINOPTS "b")) != -1)
    {
        switch (opt)
        {
            case 'b':
                brief = 1;
                break;
            case '?':
                exit(EXIT_FAILURE);
            default:
                fasta_main_getopt(opt, optarg);
        }
    }
    return optind;
}

void fasta_file_begin(const char *path, FILE *stream)
{
    (void) stream;
    printf("\n%s:\n", path);
}

void fasta_file_end(void)
{
    printf(STRFMT ": %zu\n", STRWIDTH, "count", count);
    printf(STRFMT ": %zu\n", STRWIDTH, "min", min);
    printf(STRFMT ": %zu\n", STRWIDTH, "max", max);
    count = min = max = 0;
}

int fasta_process_seq(const char *id, const char *seq)
{
    size_t len = strlen(seq);
    if (!count++)
        min = len;

    if (len < min) min = len;
    if (len > max) max = len;

    if (!brief)
        printf(STRFMT ": %zu\n", STRWIDTH, id, len);
    return FASTA_OK;
}