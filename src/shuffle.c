#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

int main_config = MAIN_NO_STDIN;

static struct fasta_reader rd;

static void
*xmalloc(size_t sz)
{
    void *p = malloc(sz);
    if (!p) {
        fprintf(stderr, "memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

static void
*xrealloc(void *p, size_t sz)
{
    p = realloc(p, sz);
    if (!p) {
        fprintf(stderr, "memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

static void
permute(int *x, int n)
{
    int i, r;

    for (i = 0; i < n; i++) {
        x[i] = -1;
    }

    for (i = 0; i < n; i++) {
        for (r = rand() % n; x[r] >= 0; r = (r + 1) % n)
            ;
        x[r] = i;
    }
}

static gzFile stream = NULL;

static struct
{
    long *p;
    long last;
    size_t n, sz;
} pos;


int
tool_init(void)
{
    fasta_reader_init(&rd, 0);
    srand(time(NULL));
    return FASTA_OK;
}

void
tool_destroy(void)
{
    fasta_reader_free(&rd);
    free(pos.p);
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
tool_file_begin(const char *path, gzFile newstream)
{
    (void) path;
    stream = newstream;
    pos.n = 0;
    pos.last = 0;
}

void
tool_file_end(void)
{
    size_t i;
    int *perm = xmalloc(sizeof *perm * pos.n);
    permute(perm, pos.n);
    for (i = 0; i < pos.n; i++) {
        if (gzseek(stream, pos.p[perm[i]], SEEK_SET) == -1) {
            perror("fseek() error in input stream");
            exit(EXIT_FAILURE);
        }
        free(rd.line);
        rd.line = NULL;
        rd.line_len = 0;
        if (fasta_read(stream, &rd) != FASTA_OK) {
            fprintf(stderr, "error reading input stream\n");
            exit(EXIT_FAILURE);
        }
        fasta_write(stdout, rd.header, rd.comment, rd.seq, main_width);
    }
    free(perm);
}

int
tool_process_seq(const char *id, const char *comment, const char *seq)
{
    (void) id;
    (void) comment;
    (void) seq;

    if (pos.n == INT_MAX) {
        fprintf(stderr, "too many sequences\n");
        exit(EXIT_FAILURE);
    }

    if (pos.n >= pos.sz) {
        pos.sz += 1024;
        pos.p = xrealloc(pos.p, sizeof *pos.p * pos.sz);
    }
    if (pos.last) {
        pos.last -= strlen(id) + 2;
    }
    pos.p[pos.n] = pos.last;
    pos.n++;

    if ((pos.last = gztell(stream)) == -1) {
        return FASTA_EBADF;
    }
    return FASTA_OK;
}
