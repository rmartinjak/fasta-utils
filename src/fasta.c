#include "fasta.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

static long
gzgetline(char **lineptr, size_t *n, gzFile stream)
{
    char buf[4097];
    size_t len, total = 0;

    do {
        if (!gzgets(stream, buf, sizeof buf)) {
            if (!total) {
                return -1;
            }
            break;
        }
        len = strlen(buf);
        if (!*lineptr || *n < total + len) {
            void *tmp = realloc(*lineptr, total + len);
            if (!tmp) {
                return -1;
            }
            *lineptr = tmp;
            *n = total + len;
        }
        memcpy(*lineptr + total, buf, len);
        total += len - 1;
        (*lineptr)[total + 1] = '\0';
    } while (buf[len - 1] != '\n');

    return total + 1;
}

void
fasta_reader_init(struct fasta_reader *rd, size_t seq_sz_hint)
{
    static struct fasta_reader zero;
    *rd = zero;
    rd->seq_sz_hint = seq_sz_hint;
}

void
fasta_reader_free(struct fasta_reader *rd)
{
    free(rd->line);
    free(rd->header);
    free(rd->seq);
}

static void
reader_getline(gzFile stream, struct fasta_reader *rd)
{
    rd->line_len = gzgetline(&rd->line, &rd->line_sz, stream);
    rd->line_no++;
}

int
fasta_read(gzFile stream, struct fasta_reader *rd)
{
    size_t len, total_len;
    if (!rd->header) {
        rd->header_sz = 0;
    }
    if (!rd->comment) {
        rd->comment_sz = 0;
    }
    if (!rd->seq) {
        if (rd->seq_sz) {
            rd->seq = malloc(rd->seq_sz_hint);
            if (!rd->seq) {
                return FASTA_ENOMEM;
            }
        }
        rd->seq_sz = rd->seq_sz_hint;
    }

    if (!rd->line) {
        reader_getline(stream, rd);
    }

    if (rd->line_len == -1) {
        return FASTA_EOF;
    }

    /* header needs at least '>', one character, '\n' */
    if (rd->line_len < 3 || rd->line[0] != '>') {
        return FASTA_EINVAL;
    }

    len = rd->line_len - 1;
    if (rd->header_sz < len) {
        void *tmp = realloc(rd->header, len);
        if (!tmp) {
            return FASTA_ENOMEM;
        }
        rd->header = tmp;
        rd->header_sz = len;
    }
    memcpy(rd->header, rd->line + 1, len - 1);
    rd->header[len - 1] = '\0';

    reader_getline(stream, rd);
    if (rd->line_len == -1) {
        return FASTA_EINVAL;
    }

    for (total_len = 0;
         rd->line[0] == ';' && rd->line_len != -1;
         reader_getline(stream, rd))
    {
        len = rd->line_len;
        if (rd->comment_sz < total_len + len) {
            void *tmp = realloc(rd->comment, total_len + len);
            if (!tmp) {
                return FASTA_ENOMEM;
            }
            rd->comment = tmp;
            rd->comment_sz = total_len + len;
        }
        /* skip leading ';' but keep newline */
        memcpy(rd->comment + total_len, rd->line + 1, len - 1);
        total_len += len - 1;
    }
    if (total_len > 1) {
        /* remove last newline */
        rd->comment[total_len - 1] = '\0';
    }

    for (total_len = 0;
         rd->line[0] != '>' && rd->line_len != -1;
         reader_getline(stream, rd))
    {
        len = rd->line_len;
        if (rd->seq_sz < total_len + len) {
            void *tmp = realloc(rd->seq, total_len + len);
            if (!tmp) {
                return FASTA_ENOMEM;
            }
            rd->seq = tmp;
            rd->seq_sz = total_len + len;
        }
        memcpy(rd->seq + total_len, rd->line, len - 1);
        total_len += len - 1;
        rd->seq[total_len] = '\0';
    }

    return FASTA_OK;
}


int
fasta_write(FILE *stream, const char *header, const char *comment,
            const char *seq, unsigned width)
{
#if HAVE_ZLIB
    int fd;
    gzFile gs;

    fd = dup(fileno(stream));
    if (fd == -1) {
        return FASTA_ERROR;
    }
    gs = gzdopen(fd, "wT");
    if (!gs) {
        close(fd);
        return FASTA_ERROR;
    }
    fasta_write_gz(gs, header, comment, seq, width);
    gzclose(gs);
#else
    fasta_write_gz(stream, header, comment, seq, width);
#endif
    return FASTA_OK;
}

void
fasta_write_gz(gzFile stream, const char *header, const char *comment,
               const char *seq, unsigned width)
{
    gzprintf(stream, ">%s\n", header);

    if (comment && *comment) {
        const char *c = comment, *nl;
        while (*c) {
            nl = strchr(c, '\n');
            if (!nl) {
                nl = c + strlen(c);
            }
            gzprintf(stream, ";%.*s\n", (int)(nl - c), c);
            c = nl + !!*nl;
        }
    }

    if (!width) {
        gzprintf(stream, "%s\n", seq);
        return;
    }

    while (*seq) {
        seq += gzprintf(stream, "%.*s", width, seq);
        gzputc(stream, '\n');
    }
}
