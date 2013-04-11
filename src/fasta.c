#include "fasta.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define BUFSZ 500
#define BUFINC 100

static void skipcomments(FILE *stream)
{
    int c, d;
    while ((c = fgetc(stream)) == ';')
        while ((d = fgetc(stream)) != EOF && d != '\n')
            ;
    ungetc(c, stream);
}

static int growbuf(char **buf, size_t *n)
{
    char *tmp = realloc(*buf, *n + BUFINC);
    if (!tmp)
        return FASTA_ERROR;
    *n += BUFINC;
    *buf = tmp;
    return FASTA_OK;
}

int fasta_read(FILE *stream, const char *accept,
               char **id, size_t *id_size,
               char **comment, size_t *comment_size,
               char **seq, size_t *seq_size)
{
    int c;
    size_t i;

    static const char *accept_last;
    static int accept_table[UCHAR_MAX + 1];

#define INIT_BUF(buf, sz) do {                                  \
        if (!*buf) {                                            \
            if (!(*buf = malloc(BUFSZ)))                        \
                return FASTA_ERROR;                             \
            *sz = BUFSZ;                                        \
        }                                                       \
    } while (0)

#define GROW_BUF(buf, sz) do {                                  \
        if (i >= *sz - 1)                                       \
            if (growbuf(buf, sz) != FASTA_OK)                   \
                return FASTA_ERROR;                             \
    } while (0)


    /* (re-)initialize accept table */
    if (accept && accept != accept_last)
    {
        int k;
        for (k = CHAR_MIN; k < CHAR_MAX + 1; k++)
        {
            accept_table[(unsigned char)k] = !!strchr(accept, k);
        }
        accept_last = accept;
    }


    /* initialize buffers if needed */
    INIT_BUF(id, id_size);
    INIT_BUF(seq, seq_size);
    if (comment)
        INIT_BUF(comment, comment_size);


    /* fasta ID line starts with '>', everything else is an error */
    if ((c = fgetc(stream)) != '>')
    {
        if (c == EOF)
            return FASTA_EOF;
        ungetc(c, stream);
        return FASTA_ERROR;
    }


    /* read ID */
    i = 0;
    while ((c = fgetc(stream)) != EOF && c != '\n')
    {
        GROW_BUF(id, id_size);
        (*id)[i++] = c;
    }
    (*id)[i] = '\0';


    /* read all comments */
    if (comment)
    {
        i = 0;
        **comment = '\0';
        while ((c = fgetc(stream)) == ';')
        {
            int d;

            if (i)
            {
                GROW_BUF(comment, comment_size);
                (*comment)[i++] = '\n';
            }

            while ((d = fgetc(stream)) != EOF && d != '\n')
            {
                GROW_BUF(comment, comment_size);
                (*comment)[i++] = d;
            }
        }
        (*comment)[i] = '\0';
        ungetc(c, stream);
    }
    /* or skip them */
    else
    {
        skipcomments(stream);
    }


    /* read sequence */
    i = 0;
    while ((c = toupper(fgetc(stream))) != EOF)
    {
        if (c == '\n')
        {
            int d = ungetc(fgetc(stream), stream);
            if (d == EOF || d == '>')
                break;
        }

        /* filter whitespace and unwanted characters */
        if (isspace(c) || (accept && accept_table[(unsigned char)c]))
            continue;

        GROW_BUF(seq, seq_size);
        (*seq)[i++] = c;
    }
    (*seq)[i] = '\0';

    return FASTA_OK;
}

void fasta_write(FILE *stream, const char *id, const char *comment,
                 const char *seq, unsigned width)
{
    fprintf(stream, ">%s\n", id);

    if (comment && *comment)
    {
        char c;
        fprintf(stream, ";");

        while ((c = *comment++))
        {
            putc(c, stream);
            if (c == '\n')
                fprintf(stream, ";");
        }
        putc('\n', stream);
    }

    if (!width)
    {
        fprintf(stream, "%s\n", seq);
        return;
    }

    while (*seq)
    {
        seq += fprintf(stream, "%.*s", width, seq);
        fputc('\n', stream);
    }
}
