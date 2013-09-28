#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "fasta.h"
#include "main.h"

static char *alphabet = NULL;

unsigned main_width = MAIN_DEFAULT_WIDTH;

unsigned
main_parse_uint(const char *s, const char *errmsg)
{
    char *endptr;
    int x = strtol(optarg, &endptr, 10);
    if (x < 0 || *endptr) {
        fprintf(stderr, "%s: %s\n", s, errmsg);
        exit(EXIT_FAILURE);
    }
    return x;
}

int
main_getopt(int opt, char *arg)
{
    switch (opt) {
        case 'a':
            alphabet = arg;
            break;
        case 'w':
            main_width = main_parse_uint(optarg, "invalid width");
            break;
        default:
            return FASTA_ERROR;
    }
    return FASTA_OK;
}

static int
process_file(const char *path)
{
    struct fasta_reader rd;
    FILE *stream = !strcmp(path, "-") ? stdin : fopen(path, "r");
    if (!stream) {
        fprintf(stderr, "%s: ", path);
        perror("");
        return FASTA_ERROR;
    }

    tool_file_begin(path, stream);

    fasta_reader_init(&rd);
    while (fasta_read(stream, &rd) == FASTA_OK) {
        if (tool_process_seq(rd.header, rd.comment, rd.seq) != FASTA_OK) {
            break;
        }
    }
    fasta_reader_free(&rd);
    tool_file_end();
    fclose(stream);
    return FASTA_OK;
}

int
main(int argc, char **argv)
{
    int i, opt;
    if (tool_init() != FASTA_OK) {
        return EXIT_FAILURE;
    }
    opt = tool_getopt(argc, argv);
    if (opt >= argc && !(main_config & MAIN_NO_STDIN)) {
        process_file("-");
    }
    if ((main_config & MAIN_ONE_FILE) && (argc - opt) > 1) {
        fprintf(stderr, "ignoring additional input files\n");
        argc = opt + 1;
    }
    for (i = opt; i < argc; i++) {
        process_file(argv[i]);
    }
    tool_destroy();
    return EXIT_SUCCESS;
}
