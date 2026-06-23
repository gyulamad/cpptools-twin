#pragma once

#ifdef TEST

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <ncursesw/ncurses.h>

// Shared ncurses screen setup for twin tests.
// Creates an isolated true-color screen via newterm with /dev/null output.
#ifndef NCURSES_SETUP
#define NCURSES_SETUP \
    setenv("TERM", "xterm-direct", 1); \
    FILE* devnull = fopen("/dev/null", "w"); \
    SCREEN* _screen_tmp = newterm("xterm-direct", devnull, stdin); \
    if (!_screen_tmp) { fprintf(stderr, "newterm xterm-direct failed\n"); exit(1); } \
    set_term(_screen_tmp); \
    start_color(); use_default_colors()

#ifndef NCURSES_TEARDOWN
#define NCURSES_TEARDOWN \
    delscreen(_screen_tmp)
#endif // NCURSES_TEARDOWN

#endif // NCURSES_SETUP

#endif // TEST
