#include "fasta.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BUFSZ 500
#define BUFINC 100

static void fasta_skipcomments(FILE *stream)
{
    int c, d;
    while ((c = fgetc(stream)) == ';')
        while ((d = fgetc(stream)) != EOF && d != '\n')
            ;
    ungetc(c, stream);
}

static int fasta_growbuf(char **buf, size_t *n)
{
    char *tmp = realloc(*buf, *n + BUFINC);
    if (!tmp)
        return FASTA_ERROR;
    *n += BUFINC;
    *buf = tmp;
    return FASTA_OK;
}

int fasta_read(FILE *stream, const char *accept, char **id,
               size_t *id_size, char **seq, size_t *seq_size)
{
    int c;
    size_t i;

    if (!*id)
        *id_size = 0;
    if (!*seq)
        *seq_size = 0;

    /* initialize buffers if needed */
    if (!*id_size && !(*id = malloc(BUFSZ)))
    {
        return FASTA_ERROR;
    }
    *id_size = BUFSZ;

    if (!*seq_size && !(*seq = malloc(BUFSZ)))
    {
        return FASTA_ERROR;
    }
    *seq_size = BUFSZ;


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
        if (i >= *id_size - 1)
        {
            if (fasta_growbuf(id, id_size) != FASTA_OK)
                return FASTA_ERROR;
        }
        (*id)[i] = c;
        i++;
    }
    (*id)[i] = '\0';


    /* skip all comment lines */
    fasta_skipcomments(stream);


    /* read sequence */
    i = 0;
    while ((c = fgetc(stream)) != EOF)
    {
        if (c == '\n')
        {
            int d = ungetc(fgetc(stream), stream);
            if (d == EOF || d == '>')
                break;

            /* always skip newlines */
        }


        /* filter whitespace and unwanted characters */
        if (isspace(c) || (accept && !strchr(accept, c)))
            continue;

        if (i >= *seq_size - 1)
        {
            if (fasta_growbuf(seq, seq_size) != FASTA_OK)
                return FASTA_ERROR;
        }
        (*seq)[i] = c;
        i++;
    }
    (*seq)[i] = '\0';

    return FASTA_OK;
}

void fasta_write(FILE *stream, const char *id, const char *comment,
                 const char *seq, int width)
{
    fprintf(stream, ">%s\n", id);

    if (comment)
        fprintf(stream, "; %s\n", comment);

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
