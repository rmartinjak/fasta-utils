#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

int config = 0;


static int n, count = 10;

int fasta_init(void)
{
    return FASTA_OK;
}

int fasta_getopt(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, FASTA_MAINOPTS "n:")) != -1)
    {
        switch (opt)
        {
            case 'n':
                count = fasta_parse_uint(optarg, "invalid number of lines");
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
    (void) path;
    (void) stream;
    n = count;
}

void fasta_file_end(void)
{
}

int fasta_process_seq(const char *id, const char *comment, const char *seq)
{
    if (!n--)
        return FASTA_CANCEL;

    fasta_write(stdout, id, comment, seq, main_width);
    return FASTA_OK;
}
