#ifndef FASTA_H
#define FASTA_H

#include <stdlib.h>
#include <stdio.h>

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
    size_t header_len, comment_len, seq_len;
    size_t header_sz, comment_sz, seq_sz, seq_sz_hint;
};

void fasta_reader_init(struct fasta_reader *rd, size_t seq_sz_hint);
void fasta_reader_free(struct fasta_reader *rd);
int fasta_read(FILE *stream, struct fasta_reader *rd);


void fasta_write(FILE *stream, const char *id, const char *comment,
                 const char *seq, unsigned width);
#endif
