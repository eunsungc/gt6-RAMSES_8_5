#include <stdarg.h>
#include <stdio.h>

/**
 * @file globus_test_tap.h
 * @brief Test Anything Protocol implementation
 */

static int total = 0;
static int failed = 0;
static int skipped = 0;
static void ok(int predval, const char *fmt, ...)
{
    static int testno=1;
    va_list ap;
    printf("%s %d - ", predval ? "ok" : "not ok", testno++);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
    if (!predval)
    {
        failed++;
    }
    total++;
}

#if __STDC_VERSION__ >= 199901L
#define get_explanationok(predval, ...) __VA_ARGS__

/* This only works if the second parameter is a call to the ok function */
#define skip(skip_predicate, ...) \
    if (skip_predicate) \
    { \
        ok(1, " # SKIP (" #skip_predicate  ") " get_explanation ## __VA_ARGS__); \
        skipped++; \
    } \
    else \
    { \
        __VA_ARGS__; \
    }
#else
#define skip(skip_predicate, okcall) \
    if (skip_predicate) \
    { \
        ok(1, " # SKIP (" #skip_predicate  ") "); \
        skipped++; \
    } \
    else \
    { \
        okcall; \
    }
#endif

#define TEST_EXIT_CODE (skipped == total) ? 77 : failed
