#pragma once

#include <cstdio>
#include <stdexcept>

template <typename T = void>
void expect_0(bool expr, const char* msg, const char* file, int line) {
    if (!expr) {
        printf("\033[0;31m%s:%d\033[0m %s\n", file, line, msg);
        throw std::runtime_error("");
    }
}

#define Expect(expr) expect_0(expr, #expr, __FILE__, __LINE__)
