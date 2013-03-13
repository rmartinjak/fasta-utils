#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <regex.h>

#include "fasta.h"
#include "main.h"

int config = 0;


#define EXPR_MAX 128

/* match ID */
static regex_t i_expr[EXPR_MAX];
static int i_expr_count = 0, i_invert = 0;

/* match comment */
static regex_t c_expr[EXPR_MAX];
static int c_expr_count = 0, c_invert = 0;

/* match sequence */
static regex_t s_expr[EXPR_MAX];
static int s_expr_count = 0, s_invert = 0;


static int expr_flags = REG_NOSUB | REG_NEWLINE;
static int match_flags = 0;

/* id/comment/seq must match ALL given expressions */
#define MATCH_AND (1 << 0)


static int expr_add(regex_t *expr, int *count, const char *regex, int flags)
{
    if (*count == EXPR_MAX)
    {
        fprintf(stderr, "too many expressions\n");
        return -1;
    }

    if (regcomp(&expr[*count], regex, expr_flags | flags))
    {
        fprintf(stderr, "invalid expression \"%s\"\n", regex);
        return -1;
    }
    (*count)++;
    return 0;
}

static int expr_match(regex_t *expr, int count, const char *string)
{
    int i;

    if (!count)
        return (match_flags & MATCH_AND) ? 1 : 0;

    for (i = 0; i < count; i++)
    {
        int match = !regexec(&expr[i], string, 0, NULL, 0);

        if ((match_flags & MATCH_AND) && !match)
            return 0;
        else if (!(match_flags & MATCH_AND) && match)
            return 1;
    }
    if (match_flags & MATCH_AND)
        return 1;
    return 0;
}


int fasta_init(void)
{
    return FASTA_OK;
}

int fasta_getopt(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, FASTA_MAINOPTS "IAEvV:i:s:c:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                if (expr_add(i_expr, &i_expr_count, optarg, 0))
                    exit(EXIT_FAILURE);
                break;
            case 'c':
                if (expr_add(c_expr, &c_expr_count, optarg, 0))
                    exit(EXIT_FAILURE);
                break;
            case 's':
                if (expr_add(s_expr, &s_expr_count, optarg, REG_ICASE))
                    exit(EXIT_FAILURE);
                break;
            case 'v':
                i_invert = c_invert = s_invert = 1;
                break;
            case 'V':
                if (!strcmp("i", optarg) || !strcmp("id", optarg))
                    i_invert = 1;
                else if (!strcmp("c", optarg) || !strcmp("comment", optarg))
                    c_invert = 1;
                else if (!strcmp("s", optarg) || !strcmp("seq", optarg) || !strcmp("sequence", optarg))
                    s_invert = 1;
                else
                {
                    fprintf(stderr, "invalid argument to -V. available are:\n"
                            "i|id    c|comment     s|seq|sequence\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'A':
                match_flags |= MATCH_AND;
                break;
            case 'I':
                expr_flags |= REG_ICASE;
                break;
            case 'E':
                expr_flags |= REG_EXTENDED;
                break;
            case '?':
                exit(EXIT_FAILURE);
            default:
                fasta_main_getopt(opt, optarg);
        }
    }

    if (!i_expr_count && !c_expr_count && !s_expr_count)
    {
        fprintf(stderr, "please specify at least one expression with -I, -C or -S\n");
        exit(EXIT_FAILURE);
    }

    return optind;
}

void fasta_file_begin(const char *path, FILE *stream)
{
    (void) path;
    (void) stream;
}

void fasta_file_end(void)
{
}

int fasta_process_seq(const char *id, const char *comment, const char *seq)
{
    int match, i_match, c_match, s_match;

    i_match = expr_match(i_expr, i_expr_count, id) ^ i_invert;
    c_match = expr_match(c_expr, c_expr_count, comment) ^ c_invert;
    s_match = expr_match(s_expr, s_expr_count, seq) ^ s_invert;

    if ((match_flags & MATCH_AND))
        match = i_match & c_match & s_match;
    else
        match = i_match | c_match | s_match;

    if (match)
        fasta_write(stdout, id, comment, seq, main_width);
    return FASTA_OK;
}
