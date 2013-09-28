#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

int main_config = 0;

static int brief = 0;
static unsigned long count, min, max, total;

#define STRFMT "%-*.*s"
#define STRWIDTH 30, 30

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
    while ((opt = getopt(argc, argv, MAIN_OPTS "b")) != -1) {
        switch (opt) {
            case 'b':
                brief = 1;
                break;
            case '?':
                exit(EXIT_FAILURE);
            default:
                main_getopt(opt, optarg);
        }
    }
    return optind;
}

void
tool_file_begin(const char *path, FILE *stream)
{
    (void) stream;
    printf("\n%s:\n", path);
}

void
tool_file_end(void)
{
    printf(STRFMT ": %lu\n", STRWIDTH, "count", count);
    printf(STRFMT ": %lu\n", STRWIDTH, "min", min);
    printf(STRFMT ": %lu\n", STRWIDTH, "max", max);
    printf(STRFMT ": %lu\n", STRWIDTH, "total", total);
    count = min = max = total = 0;
}

int
tool_process_seq(const char *id, const char *comment, const char *seq)
{
    size_t len = strlen(seq);
    (void) comment;

    if (!count++) {
        min = len;
    }

    if (len < min) {
        min = len;
    }
    if (len > max) {
        max = len;
    }
    total += len;

    if (!brief) {
        printf(STRFMT ": %lu\n", STRWIDTH, id, (unsigned long) len);
    }
    return FASTA_OK;
}
