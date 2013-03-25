#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

int main_config = 0;


static int n, count = 10;

int tool_init(void)
{
    return FASTA_OK;
}

int tool_getopt(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, MAIN_OPTS "n:")) != -1)
    {
        switch (opt)
        {
            case 'n':
                count = main_parse_uint(optarg, "invalid number of lines");
                break;

            case '?':
                exit(EXIT_FAILURE);

            default:
                main_getopt(opt, optarg);
        }
    }
    return optind;
}

void tool_file_begin(const char *path, FILE *stream)
{
    (void) path;
    (void) stream;
    n = count;
}

void tool_file_end(void)
{
}

int tool_process_seq(const char *id, const char *comment, const char *seq)
{
    if (!n--)
        return FASTA_CANCEL;

    fasta_write(stdout, id, comment, seq, main_width);
    return FASTA_OK;
}
