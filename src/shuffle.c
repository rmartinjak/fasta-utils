#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

int main_config = MAIN_NO_STDIN;


static FILE *stream = NULL;

struct line
{
    long pos;
    struct line *prev, *next;
};


static int lines = 1, pos = 0;
static struct line root = { -1, &root, &root };


static struct line *line_get(struct line *ln, unsigned n)
{
    while (n--)
        ln = ln->next;
    return ln;
}

static struct line *line_add(struct line *ln, long pos)
{
    struct line *new = malloc(sizeof *new);
    if (new)
    {
        new->pos = pos;

        new->next = ln->next;
        new->prev = ln;

        ln->next->prev = new;
        ln->next = new;
    }
    return new;
}

static void line_del(struct line *ln)
{
    ln->prev->next = ln->next;
    ln->next->prev = ln->prev;
}


int tool_init(void)
{
    srand(time(NULL));
    return FASTA_OK;
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
    lines = 1;
    pos = 0;
}

void tool_file_end(void)
{
    char *id = NULL, *comment = NULL, *seq = NULL;
    size_t id_n, comment_n, seq_n;

    if (!stream)
        return;

    while (root.next != &root)
    {
        struct line *ln = line_get(&root, rand() % lines);

        if (ln == &root)
            continue;

        if (!fseek(stream, ln->pos, SEEK_SET) &&
            fasta_read(stream, NULL, &id, &id_n, &comment, &comment_n, &seq, &seq_n) == FASTA_OK)
        {
            fasta_write(stdout, id, comment, seq, main_width);
        }

        line_del(ln);
    }
    free(id);
    free(comment);
    free(seq);
}

int tool_process_seq(const char *id, const char *comment, const char *seq)
{
    (void) id;
    (void) comment;
    (void) seq;
    line_add(&root, pos);
    lines++;
    pos = ftell(stream);
    if (pos == -1)
        return FASTA_ERROR;
    return FASTA_OK;
}
