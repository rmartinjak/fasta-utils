#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

int main_config = MAIN_ONE_FILE;


#define SUFFIXLEN_DEFAULT 2
#define SUFFIXLEN_MAX 100

static int count = 1000;

static char *prefix = "x";
static char suffix[SUFFIXLEN_MAX + 1];
static char *ext = "fasta";

static FILE *outstream = NULL;

static char *
next_suffix(void)
{
    char *p = suffix + strlen(suffix) - 1;

    while (p > suffix)
    {
        if (*p != 'z')
        {
            *p += 1;
            return suffix;
        }
        *p-- = 'a';
    }
    if (*p == 'z')
    {
        fprintf(stderr, "output file suffixes exhausted\n");
        return NULL;
    }

    *p += 1;
    return suffix;
}

static int
next_outfile(void)
{
    char path[1024];

    if (outstream) {
        fclose(outstream);
    }

    if (strlen(prefix) + strlen(suffix) + 1 + strlen(ext) + 1 > sizeof path) {
        fprintf(stderr, "prefix+suffix+.ext too long\n");
        exit(EXIT_FAILURE);
    }

    sprintf(path, "%s%s%s%s", prefix, suffix, (*ext ? "." : ""), ext);
    outstream = fopen(path, "w");

    if (!outstream) {
        fprintf(stderr, "%s: ", path);
        perror("");
        return FASTA_ERROR;
    }

    return FASTA_OK;
}

int
tool_init(void)
{
    return FASTA_OK;
}

void
tool_destroy(void)
{
}

int
tool_getopt(int argc, char **argv)
{
    int opt;
    unsigned suffixlen = SUFFIXLEN_DEFAULT;
    while ((opt = getopt(argc, argv, MAIN_OPTS "n:p:s:e:")) != -1) {
        switch (opt) {
            case 'n':
                count = main_parse_uint(optarg, "invalid number of lines");
                break;
            case 'p':
                prefix = optarg;
                break;
            case 's':
                suffixlen = main_parse_uint(optarg, "invalid suffix length");
                break;
            case 'e':
                ext = optarg;
                break;
            case '?':
                exit(EXIT_FAILURE);
            default:
                main_getopt(opt, optarg);
        }
    }
    if (!suffixlen || suffixlen > SUFFIXLEN_DEFAULT) {
        fprintf(stderr, "invalid suffix length: %u\n", suffixlen);
        exit(EXIT_FAILURE);
    }
    memset(suffix, 'a', suffixlen);
    suffix[suffixlen] = '\0';
    if (next_outfile() != FASTA_OK) {
        exit(EXIT_FAILURE);
    }
    return optind;
}

void
tool_file_begin(const char *path, FILE *stream)
{
    (void) path;
    (void) stream;
}

void
tool_file_end(void)
{
}

int
tool_process_seq(const char *id, const char *comment, const char *seq)
{
    static int n = 0;
    if (++n > count) {
        if (!next_suffix() || next_outfile() != FASTA_OK) {
            return FASTA_ERROR;
        }
        n = 1;
    }
    fasta_write(outstream, id, comment, seq, main_width);
    return FASTA_OK;
}
