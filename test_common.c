#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bench.c"
#include "bloom.h"
#include "tst.h"

#define TableSize 5000000 /* size of bloom filter */
#define HashNumber 2      /* number of hash functions */
#define BENCH_TEST_FILE "bench_ref.txt"
#define IN_FILE "cities.txt"

typedef enum mode Mode;
enum mode {
    INPUT_MODE = 0,
    BENCH_MODE = 1,
    FILE_MODE = 2,
    CMD_MODE = 3,
};

/** constants insert, delete, max word(s) & stack nodes */
enum { INS, DEL, WRDMAX = 256, STKMAX = 512, LMAX = 1024 };

int REF = INS;
long poolsize = 2000000 * WRDMAX;

/* simple trim '\n' from end of buffer filled by fgets */
static void rmcrlf(char *s)
{
    size_t len = strlen(s);
    if (len && s[len - 1] == '\n')
        s[--len] = 0;
}

int main(int argc, char **argv)
{
    char word[WRDMAX] = "";
    char buf[WORDMAX];
    char *sgl[LMAX] = {NULL};
    tst_node *root = NULL, *res = NULL;
    int idx = 0, sidx = 0;
    double t1, t2;
    int CPYmask = -1;

    if (argc < 2) {
        printf("too less argument\n");
        return -1;
    }

    int option_index = 0;
    struct option opts[] = {
        {"bench", 0, NULL, 'b'},
        {"file", 1, NULL, 'f'},
        {"cmd", 1, NULL, 'c'},
    };

    Mode mode = INPUT_MODE;
    int c;
    char paramstr[WRDMAX];
    while ((c = getopt_long(argc, argv, "bpf:c:", opts, &option_index)) != -1) {
        switch (c) {
        case 'b':
            if (!mode)
                mode = BENCH_MODE;
            else {
                fprintf(stderr, "Too many option.\n");
                return -1;
            }
            break;
        case 'f':
            if (!mode) {
                mode = FILE_MODE;
                strncpy(paramstr, optarg, WRDMAX);
                paramstr[WRDMAX - 1] = '\0';
            } else {
                fprintf(stderr, "Too many option.\n");
                return -1;
            }
            break;
        case 'c':
            if (!mode) {
                mode = CMD_MODE;
                strncpy(paramstr, optarg, WRDMAX);
                paramstr[WRDMAX - 1] = '\0';
            } else {
                fprintf(stderr, "Too many option.\n");
                return -1;
            }
            break;
        default:
            printf("Unknown option\n");
            break;
        }
    }

    int flag = 0;
    for (int index = optind; index < argc; index++) {
        if (!strcmp(argv[index], "CPY")) {
            CPYmask = 0;
            REF = DEL;
            printf("CPY mechanism\n");
            flag = 1;
        } else if (!strcmp(argv[index], "REF")) {
            printf("REF mechanism\n");
            flag = 1;
        }
    }

    if (flag == 0) {
        printf("Must defined using CPY or REF\n");
        return -1;
    }


    FILE *fp = fopen(IN_FILE, "r");
    if (!fp) { /* prompt, open, validate file for reading */
        fprintf(stderr, "error: file open failed '%s'.\n", argv[1]);
        return 1;
    }

    t1 = tvgetf();
    tst_init();
    bloom_t bloom = bloom_create(TableSize);
    char *Top = word;
    char *pool;

    if (CPYmask) {
        /* memory pool */
        pool = (char *) malloc(poolsize * sizeof(char));
        Top = pool;
    }

    while (fgets(buf, WORDMAX, fp)) {
        int offset = 0;
        for (int i = 0, j = 0; buf[i + offset]; i++) {
            Top[i] =
                (buf[i + j] == ',' || buf[i + j] == '\n') ? '\0' : buf[i + j];
            j += (buf[i + j] == ',');
        }
        while (*Top) {
            if (!tst_ins_del(&root, Top, INS, REF)) { /* fail to insert */
                fprintf(stderr, "error: memory exhausted, tst_insert.\n");
                fclose(fp);
                return 1;
            }
            bloom_add(bloom, Top);
            idx++;
            int len = strlen(Top);
            offset += len + 1;
            Top += len + 1;
        }
        Top -= offset & ~CPYmask;
        memset(Top, '\0', WORDMAX);
    }
    t2 = tvgetf();

    fclose(fp);
    printf("ternary_tree, loaded %d words in %.6f sec\n", idx, t2 - t1);

    FILE *logfile = NULL;
    int stat = 0;
    switch (mode) {
    case BENCH_MODE:
        stat = bench_test(root, BENCH_TEST_FILE, LMAX);
        // after test, free all allocated memory
        free(pool);
        if (CPYmask)
            tst_free(root);
        else
            tst_free_all(root);

        pool_free_all(&tst_node_pool);
        bloom_free(bloom);
        return stat;
    case FILE_MODE:
        logfile = fopen(paramstr, "r");
        if (!logfile) {
            fprintf(stderr, "error: file open failed '%s'.\n", paramstr);
            return 1;
        }
        break;
    default:
        break;
    }

    for (;;) {
        if (mode == INPUT_MODE)
            printf(
                "\nCommands:\n"
                " a  add word to the tree\n"
                " f  find word in tree\n"
                " s  search words matching prefix\n"
                " d  delete word from the tree\n"
                " q  quit, freeing all data\n\n"
                "choice: ");

        if (mode == INPUT_MODE)
            fgets(buf, WORDMAX, stdin);
        else if (mode == FILE_MODE) {
            if (!fgets(buf, WORDMAX, logfile))
                break;
        } else if (mode == CMD_MODE)
            strncpy(buf, paramstr, 1);
        /* else case may not happen...... */

        switch (*buf) {
        case 'a':
            printf("\nenter word to add: ");
            if (mode == FILE_MODE)
                strcpy(Top, &buf[2]);
            else if (mode == CMD_MODE)
                strcpy(Top, &paramstr[1]);
            else if (!fgets(Top, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(Top);

            t1 = tvgetf();
            if (bloom_test(bloom, Top)) /* if detected by filter, skip */
                res = NULL;
            else { /* update via tree traversal and bloom filter */
                bloom_add(bloom, Top);
                res = tst_ins_del(&root, Top, INS, REF);
            }
            t2 = tvgetf();
            if (res) {
                idx++;
                Top += (strlen(Top) + 1) & CPYmask;
                printf("  %s - inserted in %.10f sec. (%d words in tree)\n",
                       (char *) res, t2 - t1, idx);
            }
            break;
        case 'f':
            printf("\nfind word in tree: ");
            if (mode == FILE_MODE)
                strcpy(word, &buf[2]);
            else if (mode == CMD_MODE)
                strcpy(word, &paramstr[1]);
            else if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }

            rmcrlf(word);
            t1 = tvgetf();

            if (bloom_test(bloom, word)) {
                t2 = tvgetf();
                printf("  Bloomfilter found %s in %.6f sec.\n", word, t2 - t1);
                printf(
                    "  Probability of false positives:%lf\n",
                    pow(1 - exp(-(double) HashNumber /
                                (double) ((double) TableSize / (double) idx)),
                        HashNumber));
                t1 = tvgetf();
                res = tst_search(root, word);
                t2 = tvgetf();
                if (res)
                    printf("  ----------\n  Tree found %s in %.6f sec.\n",
                           (char *) res, t2 - t1);
                else
                    printf("  ----------\n  %s not found by tree.\n", word);
            } else
                printf("  %s not found by bloom filter.\n", word);
            break;
        case 's':
            printf("\nfind words matching prefix (at least 1 char): ");
            if (mode == FILE_MODE)
                strcpy(word, &buf[2]);
            else if (mode == CMD_MODE)
                strcpy(word, &paramstr[1]);
            else if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }

            rmcrlf(word);
            t1 = tvgetf();
            res = tst_search_prefix(root, word, sgl, &sidx, LMAX);
            t2 = tvgetf();
            if (res) {
                printf("  %s - searched prefix in %.6f sec\n", word, t2 - t1);
                for (int i = 0; i < sidx; i++)
                    printf("suggest[%d] : %s\n", i, sgl[i]);
            } else
                printf("  %s - not found\n", word);
            break;
        case 'd':
            printf("\nenter word to del: ");
            if (mode == FILE_MODE)
                strcpy(word, &buf[2]);
            else if (mode == CMD_MODE)
                strcpy(word, &paramstr[1]);
            else if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }

            rmcrlf(word);
            printf("  deleting %s\n", word);
            t1 = tvgetf();
            /* FIXME: remove reference to each string */
            res = tst_ins_del(&root, word, DEL, REF);
            t2 = tvgetf();
            if (res)
                printf("  delete failed.\n");
            else {
                printf("  deleted %s in %.6f sec\n", word, t2 - t1);
                idx--;
            }
            break;
        case 'q':
            goto quit;
        default:
            fprintf(stderr, "error: invalid selection.\n");
            break;
        }

        if (mode == CMD_MODE)
            break;
    }


quit:
    fclose(logfile);
    free(pool);
    if (CPYmask)
        tst_free(root);
    else
        tst_free_all(root);

    pool_free_all(&tst_node_pool);
    bloom_free(bloom);

    return 0;
}
