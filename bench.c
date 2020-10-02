#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bench.h"

#define DICT_FILE "cities.txt"
#define PREFIX_LEN 3
#define WORDMAX 256

double tvgetf()
{
    struct timespec ts;
    double sec;

    clock_gettime(CLOCK_REALTIME, &ts);
    sec = ts.tv_nsec;
    sec /= 1e9;
    sec += ts.tv_sec;

    return sec;
}

int bench_test(const tst_node *root, char *out_file, const int max)
{
    char prefix[PREFIX_LEN + 1] = "";
    char word[WORDMAX] = "";
    char **sgl;
    FILE *fp = fopen(out_file, "w");
    FILE *dict = fopen(DICT_FILE, "r");
    int idx = 0, sidx = 0;

    if (!fp || !dict) {
        if (fp) {
            fprintf(stderr, "error: file open failed in '%s'.\n", DICT_FILE);
            fclose(fp);
        }
        if (dict) {
            fprintf(stderr, "error: file open failed in '%s'.\n", out_file);
            fclose(dict);
        }
        return 1;
    }

    sgl = (char **) malloc(sizeof(char *) * max);
    struct timespec tt1, tt2;
    while (fscanf(dict, "%s", word) != EOF) {
        if (strlen(word) < sizeof(prefix) - 1)
            continue;
        strncpy(prefix, word, sizeof(prefix) - 1);
        clock_gettime(CLOCK_MONOTONIC, &tt1);
        tst_search_prefix(root, prefix, sgl, &sidx, max);
        clock_gettime(CLOCK_MONOTONIC, &tt2);

        long long time = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                         (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);
        fprintf(fp, "%d, %lld\n", idx, time);
        idx++;
    }

    free(sgl);
    fclose(fp);
    fclose(dict);
    return 0;
}
