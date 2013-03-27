#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

int main_config = MAIN_NO_STDIN;

static void *xmalloc(size_t sz)
{
    void *p = malloc(sz);
    if (!p)
    {
        fprintf(stderr, "memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

static void *xrealloc(void *p, size_t sz)
{
    p = realloc(p, sz);
    if (!p)
    {
        fprintf(stderr, "memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

static void permute(int *x, int n)
{
    int i, r;

    for (i = 0; i < n; i++)
        x[i] = -1;

    for (i = 0; i < n; i++)
    {
        for (r = rand() % n; x[r] >= 0; r = (r + 1) % n)
            ;
        x[r] = i;
    }
}

static FILE *stream = NULL;

static struct
{
    long *p;
    long last;
    size_t n, sz;
} pos;


int tool_init(void)
{
    srand(time(NULL));
    return FASTA_OK;
}

void tool_destroy(void)
{
    free(pos.p);
}

int tool_getopt(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, MAIN_OPTS)) != -1)
    {
        switch (opt)
        {
            case '?':
                exit(EXIT_FAILURE);

            default:
                main_getopt(opt, optarg);
        }
    }
    return optind;
}

void tool_file_begin(const char *path, FILE *newstream)
{
    (void) path;
    stream = newstream;
    pos.n = 0;
    pos.last = 0;
}

void tool_file_end(void)
{
    char *id = NULL, *comment = NULL, *seq = NULL;
    size_t id_n, comment_n, seq_n;
    size_t i;
    int *perm;

    perm = xmalloc(sizeof *perm * pos.n);
    permute(perm, pos.n);

    for (i = 0; i < pos.n; i++)
    {
        if (fseek(stream, pos.p[perm[i]], SEEK_SET))
        {
            perror("fseek() error in input stream");
            exit(EXIT_FAILURE);
        }

        if (fasta_read(stream, NULL, &id, &id_n, &comment, &comment_n,
                    &seq, &seq_n) != FASTA_OK)
        {
            fprintf(stderr, "error reading input stream\n");
            exit(EXIT_FAILURE);
        }

        fasta_write(stdout, id, comment, seq, main_width);
    }

    free(id);
    free(comment);
    free(seq);
    free(perm);
}

int tool_process_seq(const char *id, const char *comment, const char *seq)
{
    (void) id;
    (void) comment;
    (void) seq;

    if (pos.n == INT_MAX)
    {
        fprintf(stderr, "too many sequences\n");
        exit(EXIT_FAILURE);
    }

    if (pos.n >= pos.sz)
    {
        pos.sz += 1024;
        pos.p = xrealloc(pos.p, sizeof *pos.p * pos.sz);
    }
    pos.p[pos.n] = pos.last;
    pos.n++;

    if ((pos.last = ftell(stream)) == -1)
        return FASTA_ERROR;
    return FASTA_OK;
}
