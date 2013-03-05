#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct line
{
    long pos;
    size_t len;
    struct line *prev, *next;
};

struct line *line_get(struct line *ln, unsigned n)
{
    while (n--)
        ln = ln->next;
    return ln;
}

struct line *line_add(struct line *ln, long pos, size_t len)
{
    struct line *new = malloc(sizeof *new);
    if (new)
    {
        new->pos = pos;
        new->len = len;

        new->next = ln->next;
        new->prev = ln;

        ln->next->prev = new;
        ln->next = new;
    }
    return new;
}

void line_del(struct line *ln)
{
    ln->prev->next = ln->next;
    ln->next->prev = ln->prev;
}

void shuffle_file(const char *filename)
{
    struct line root;
    int c;
    FILE *f;
    long lines, pos;
    size_t len;

    root.next = &root;
    root.prev = &root;

    f = fopen(filename, "r");
    if (!f)
    {
        fprintf(stderr, "cannot open %s:", filename);
        perror("");
        return;
    }

    lines = 1;
    pos = 0;
    len = 0;
    while ((c = fgetc(f)) != EOF)
    {
        int next = fgetc(f);
        len++;
        if (c == '\n' && (next == '>' || next == EOF))
        {
            lines++;
            line_add(&root, pos, len);
            pos = ftell(f) - 1;
            len = 0;
        }
        ungetc(next, f);
    }

    while (root.next != &root)
    {
        struct line *r = &root;
        struct line *ln = line_get(r, rand() % lines);

        if (ln == &root)
            continue;

        fseek(f, ln->pos, SEEK_SET);
        len = ln->len;
        while (len--)
        {
            c = fgetc(f);
            putchar(c);
        }
        line_del(ln);
        free(ln);
        lines--;
    }
}

int main(int argc, char **argv)
{
    (void) argc;
    srand(time(NULL));
    while (*++argv)
        shuffle_file(*argv);

    return EXIT_SUCCESS;
}
