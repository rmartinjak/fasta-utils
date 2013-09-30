#ifndef FASTA_H
#define FASTA_H

#include <stdlib.h>
#include <stdio.h>

#if HAVE_ZLIB
#include <zlib.h>
#else
typedef FILE *gzFile;
#define gzread(stream, buf, n) ((int)fread(buf, 1, n, stream))
#define gzwrite(stream, buf, n) ((int)fwrite(buf, 1, n, stream))
#define gzgets(stream, s, size) fgets(s, size, stream)
#define gzbuffer(file, size) (void)0
#define gzopen fopen
#define gzclose fclose
#define gzprintf fprintf
#define gzputc(stream, c) putc(c, stream)
#define gzseek fseek
#define gztell ftell
#endif

enum
{
    FASTA_OK,
    FASTA_EOF,
    FASTA_ENOMEM,
    FASTA_EINVAL,
    FASTA_EBADF,
    FASTA_ERROR
};


struct fasta_reader
{
    char *line;
    long line_len;
    size_t line_sz, line_no;
    char *header, *comment, *seq;
    size_t header_sz, comment_sz, seq_sz, seq_sz_hint;
};

void fasta_reader_init(struct fasta_reader *rd, size_t seq_sz_hint);
void fasta_reader_free(struct fasta_reader *rd);
int fasta_read(gzFile stream, struct fasta_reader *rd);

void fasta_write_gz(gzFile stream, const char *header, const char *comment,
                    const char *seq, unsigned width);

int fasta_write(FILE *stream, const char *id, const char *comment,
                const char *seq, unsigned width);
#endif
