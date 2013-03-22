#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "fasta.h"
#include "main.h"

#include <assert.h>

int main_config = 0;


static int chop_width = 0;

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
                chop_width = main_parse_uint(optarg, "invalid chop width");
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
}

void tool_file_end(void)
{
}

int tool_process_seq(const char *id, const char *comment, const char *seq)
{
    if (chop_width)
    {
        char *part;
        const char *end;
        if (!(part = malloc(chop_width + 1)))
            return FASTA_ERROR;

        for (end = seq + strlen(seq); end - seq > chop_width; seq += chop_width)
        {
            memcpy(part, seq, chop_width);
            part[chop_width] = '\0';
            fasta_write(stdout, id, comment, part, main_width);
        }
    }
    fasta_write(stdout, id, comment, seq, main_width);
    return FASTA_OK;
}
