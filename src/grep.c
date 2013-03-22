#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <regex.h>

#include "fasta.h"
#include "main.h"

int main_config = 0;


#define EXPR_MAX 128

struct match
{
    regex_t re[EXPR_MAX];
    unsigned n;
    int inv;
} mi, mc, ms;

static int eflags = REG_NOSUB | REG_NEWLINE;
static int mflags = 0;

/* id/comment/seq must match ALL given expressions */
#define MATCH_AND (1 << 0)
#define MATCH_INV (1 << 1)


static int expr_add(struct match *m, const char *regex, int flags)
{
    if (m->n == EXPR_MAX)
    {
        fprintf(stderr, "too many expressions\n");
        return -1;
    }

    if (regcomp(&(m->re[m->n]), regex, eflags | flags))
    {
        fprintf(stderr, "invalid expression \"%s\"\n", regex);
        return -1;
    }
    m->n++;
    return 0;
}

static int expr_match(const struct match *m, const char *s)
{
    unsigned i;

    if (!m->n)
        return (mflags & MATCH_AND) ? 1 : 0;

    for (i = 0; i < m->n; i++)
    {
        int match = !regexec(&(m->re[i]), s, 0, NULL, 0);

        if ((mflags & MATCH_AND) && !match)
            return 0 ^ m->inv;
        else if (!(mflags & MATCH_AND) && match)
            return 1 ^ m->inv;
    }
    if (mflags & MATCH_AND)
        return 1 ^ m->inv;
    return 0 ^ m->inv;
}


int tool_init(void)
{
    return FASTA_OK;
}

int tool_getopt(int argc, char **argv)
{
    int opt;
    char *p;

    while ((opt = getopt(argc, argv, MAIN_OPTS "IAEvV:i:s:c:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                if (expr_add(&mi, optarg, 0))
                    exit(EXIT_FAILURE);
                break;

            case 'c':
                if (expr_add(&mc, optarg, 0))
                    exit(EXIT_FAILURE);
                break;

            case 's':
                if (expr_add(&ms, optarg, REG_ICASE))
                    exit(EXIT_FAILURE);
                break;

            case 'v':
                mflags |= MATCH_INV;
                break;

            case 'V':
                for (p = optarg; *p; p++)
                {
                    if (*p == 'i')
                        mi.inv = 1;
                    else if (*p == 'c')
                        mc.inv = 1;
                    else if (*p == 's')
                        ms.inv = 1;
                    else
                    {
                        fprintf(stderr, "Invalid argument to -V. Allowed: i, c, s\n");
                        exit(EXIT_FAILURE);
                    }
                }
                break;

            case 'A':
                mflags |= MATCH_AND;
                break;

            case 'I':
                eflags |= REG_ICASE;
                break;

            case 'E':
                eflags |= REG_EXTENDED;
                break;

            case '?':
                exit(EXIT_FAILURE);

            default:
                main_getopt(opt, optarg);
        }
    }

    if (!mi.n && !mc.n && !ms.n)
    {
        fprintf(stderr, "please specify at least one expression with -i, -c or -s\n");
        exit(EXIT_FAILURE);
    }

    return optind;
}

void tool_file_begin(const char *path, FILE *stream)
{
    (void) path;
    (void) stream;
}

void tool_file_end(void)
{
}

int tool_process_seq(const char *id, const char *comment, const char *seq)
{
    int match, i, c, s;

    i = expr_match(&mi, id);
    c = expr_match(&mc, comment);
    s = expr_match(&ms, seq);

    if ((mflags & MATCH_AND))
        match = i && c && s;
    else
        match = i || c || s;

    if (mflags & MATCH_INV)
        match = !match;

    if (match)
        fasta_write(stdout, id, comment, seq, main_width);
    return FASTA_OK;
}
