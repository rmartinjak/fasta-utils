#ifndef FASTA_H
#define FASTA_H

#include <stdlib.h>
#include <stdio.h>

#define FASTA_OK 0
#define FASTA_EOF -1
#define FASTA_ERROR -2
#define FASTA_CANCEL -3

int fasta_read(FILE *stream, const char *accept,
               char **id, size_t *id_size,
               char **comment, size_t *comment_size,
               char **seq, size_t *seq_size);

void fasta_write(FILE *stream, const char *id, const char *comment,
                 const char *seq, unsigned width);
#endif
