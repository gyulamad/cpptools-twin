#pragma once

#ifdef TEST

#include <string.h>
#include "../../misc/TEST.hpp"
#include "../TTheme.hpp"
#include "test_NcursesScreen.hpp"

// ============================================================================
// TTheme Tests - Color Pair Delegation
// ============================================================================

TEST(test_TTheme_getWindowColorPair_returns_value) {
    NCURSES_SETUP;
    TTheme theme;
    short pair = theme.getWindowColorPair();
    assert(pair > 0 && "getWindowColorPair should return a positive color pair index");
    NCURSES_TEARDOWN;
}

TEST(test_TTheme_getBoxColorPair_returns_value) {
    NCURSES_SETUP;
    TTheme theme;
    short pair = theme.getBoxColorPair();
    assert(pair > 0 && "getBoxColorPair should return a positive color pair index");
    NCURSES_TEARDOWN;
}

TEST(test_TTheme_getScrollbarColorPair_returns_value) {
    NCURSES_SETUP;
    TTheme theme;
    short pair = theme.getScrollbarColorPair();
    assert(pair > 0 && "getScrollbarColorPair should return a positive color pair index");
    NCURSES_TEARDOWN;
}

TEST(test_TTheme_getInputColorPair_returns_value) {
    NCURSES_SETUP;
    TTheme theme;
    short pair = theme.getInputColorPair();
    assert(pair > 0 && "getInputColorPair should return a positive color pair index");
    NCURSES_TEARDOWN;
}

TEST(test_TTheme_getPalette_returns_palette_ref) {
    NCURSES_SETUP;
    TTheme theme;
    TColorPairPalette& palette = theme.getPalette();
    assert(&palette != nullptr && "getPalette should return a valid reference");
    NCURSES_TEARDOWN;
}

TEST(test_TTheme_getColorPair_delegates_to_palette) {
    NCURSES_SETUP;
    TTheme theme;
    short pair1 = theme.getColorPair(COLOR_RED, COLOR_BLACK);
    short pair2 = theme.getColorPair(COLOR_GREEN, COLOR_WHITE);
    assert(pair1 > 0 && "First color pair should be positive");
    assert(pair2 > 0 && "Second color pair should be positive");
    assert(pair1 != pair2 && "Different colors should produce different pairs");
    NCURSES_TEARDOWN;
}

TEST(test_TTheme_same_color_pair_returns_cached_index) {
    NCURSES_SETUP;
    TTheme theme;
    short pair1 = theme.getColorPair(COLOR_RED, COLOR_BLACK);
    short pair2 = theme.getColorPair(COLOR_RED, COLOR_BLACK);
    assert(pair1 == pair2 && "Same color combination should return the same cached index");
    NCURSES_TEARDOWN;
}

#endif