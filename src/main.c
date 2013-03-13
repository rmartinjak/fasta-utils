#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

static char *alphabet = NULL;

int main_width = FASTA_DEFAULTWIDTH;

unsigned fasta_parse_uint(const char *s, const char *errmsg)
{
    char *endptr;
    int x = strtol(optarg, &endptr, 10);
    if (x < 0 || *endptr)
    {
        fprintf(stderr, "%s: %s\n", s, errmsg);
        exit(EXIT_FAILURE);
    }
    return x;
}

int fasta_main_getopt(int opt, char *arg)
{
    switch (opt)
    {
        case 'a':
            alphabet = arg;
            break;

        case 'w':
            main_width = fasta_parse_uint(optarg, "invalid width");
            break;

        default:
            return FASTA_ERROR;
    }
    return FASTA_OK;
}


static int process_file(const char *path)
{
    char *id = NULL, *comment = NULL, *seq = NULL;
    size_t id_n, comment_n, seq_n;
    FILE *stream = !strcmp(path, "-") ? stdin : fopen(path, "r");
    if (!stream)
    {
        fprintf(stderr, "%s: ", path);
        perror("");
        return FASTA_ERROR;
    }

    fasta_file_begin(path, stream);

    while (fasta_read(stream, alphabet, &id, &id_n, &comment, &comment_n, &seq, &seq_n) == FASTA_OK)
    {
        if (fasta_process_seq(id, comment, seq) == FASTA_CANCEL)
            break;
    }

    fasta_file_end();

    free(id);
    free(comment);
    free(seq);
    fclose(stream);
    return FASTA_OK;
}

int main(int argc, char **argv)
{
    int i, opt;

    if (fasta_init() != FASTA_OK)
        return EXIT_FAILURE;

    opt = fasta_getopt(argc, argv);

    if (opt >= argc && !(config & FASTA_NOSTDIN))
        process_file("-");

    if ((config & FASTA_ONEFILE) && (argc - opt) > 1)
    {
        fprintf(stderr, "ignoring additional input files\n");
        argc = opt + 1;
    }

    for (i = opt; i < argc; i++)
        process_file(argv[i]);

    return EXIT_SUCCESS;
}
