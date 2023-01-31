#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>


void format_binary_string(char body[17], uint16_t n) { //{{{
    uint16_t mask = (uint16_t)(1<<15);
    for (int i = 0; i < 16; ++i) {
        if (mask & n) body[i] = '1';
        else body[i] = '0';
        mask >>= 1;
    }
    body[16]='\0';
/*}}}*/}

void print_binary(uint16_t n) { //{{{
    uint16_t mask = 1<<15;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (mask & n) printf("1");
            else printf("0");
            mask >>= 1;
        }
        printf(" ");
    }
/*}}}*/}

static unsigned int tested, passed;

bool assert(bool b, const char* format, ...) { //{{{
    va_list args;
    va_start(args, format);

    tested++;
    if (!b) {
        fprintf(stderr, "\033[91mFAIL:\033[0m ");
        vfprintf(stderr, format, args);
    }
    else {
        passed++;
        printf("\033[92mPASS:\033[0m ");
        vprintf(format, args);
    }
    va_end(args);
    printf("\n");
    return b;

/*}}}*/ }

void print_test_report() { //{{{
    unsigned int failed = tested - passed;
    if (failed) {
        printf(
            "\033[93m%d TESTS\033[0m  /  \033[92m%d PASSED\033[0m  /  \033[91m%d FAILED\033[0m\n",
            tested, passed, failed
        );
    }
    else {
        printf("\033[42m%d TESTS PASSED\033[0m\n", tested);
    }
/*}}}*/ }

#endif