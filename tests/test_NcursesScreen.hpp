#pragma once

#ifdef TEST

#include <string.h>

// Shared ncurses screen setup for twin tests.
// Creates an isolated screen via newterm with /dev/null output and initializes colors.
#ifndef NCURSES_SETUP
#define NCURSES_SETUP \
    setenv("TERM", "xterm-256color", 1); \
    FILE* devnull = fopen("/dev/null", "w"); \
    SCREEN* screen = newterm("xterm-256color", devnull, stdin); \
    start_color(); use_default_colors()

#ifndef NCURSES_TEARDOWN
#define NCURSES_TEARDOWN \
    delscreen(screen)
#endif // NCURSES_TEARDOWN

#endif // NCURSES_SETUP

#endif // TEST
