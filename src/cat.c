#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "fasta.h"

#define COUNT_DEFAULT 10U
#define WIDTH_DEFAULT 0

void fasta_print_seqs(FILE *f, unsigned width, unsigned n)
{
    char *id = NULL, *seq = NULL;
    size_t id_n, seq_n;

    while (fasta_read(f, NULL, &id, &id_n, &seq, &seq_n) == FASTA_OK)
    {
#ifdef FASTA_HEAD
        if (!n--)
            break;
#else
        (void) n;
#endif
        fasta_write(stdout, id, NULL, seq, width);
    }
    free(id);
    free(seq);
}

static unsigned parse_int(const char *s, const char *errmsg)
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


#ifdef FASTA_HEAD
#define OPTSTRING "w:n:"
#else
#define OPTSTRING "w:"
#endif
int main(int argc, char **argv)
{
    int i, opt;
    unsigned width = WIDTH_DEFAULT, count = COUNT_DEFAULT;

    while ((opt = getopt(argc, argv, OPTSTRING)) != -1)
    {
        if (opt == 'w')
            width = parse_int(optarg, "invalid width");
#ifdef FASTA_HEAD
        if (opt == 'n')
            count = parse_int(optarg, "invalid number of lines");
#endif
        if (opt == '?')
            exit(EXIT_FAILURE);
    }

    if (optind >= argc)
    {
        fasta_print_seqs(stdin, width, count);
        return EXIT_SUCCESS;
    }

    for (i = optind; i < argc; i++)
    {
        FILE *f = !strcmp(argv[i], "-") ? stdin : fopen(argv[i], "r");
        if (!f)
        {
            fprintf(stderr, "%s: ", argv[i]);
            perror("");
        }
        else
        {
            fasta_print_seqs(f, width, count);
            fclose(f);
        }
    }

    return EXIT_SUCCESS;
}
