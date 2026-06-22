#pragma once

#ifdef TEST

#include <string.h>
#include "../../misc/TEST.hpp"
#include "../TColorPairPalette.hpp"
#include "test_NcursesScreen.hpp"

// ============================================================================
// TColorPairPalette Tests - Color Pair Management
// ============================================================================

TEST(test_TColorPairPalette_getColorPair_returns_positive_index) {
    NCURSES_SETUP;
    TColorPairPalette palette;
    short pair = palette.getColorPair(COLOR_RED, COLOR_BLACK);
    assert(pair > 0 && "getColorPair should return a positive index");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPairPalette_same_colors_return_cached_index) {
    NCURSES_SETUP;
    TColorPairPalette palette;
    short pair1 = palette.getColorPair(COLOR_RED, COLOR_BLACK);
    short pair2 = palette.getColorPair(COLOR_RED, COLOR_BLACK);
    assert(pair1 == pair2 && "Same color combination should return cached index");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPairPalette_different_colors_return_different_indices) {
    NCURSES_SETUP;
    TColorPairPalette palette;
    short pair1 = palette.getColorPair(COLOR_RED, COLOR_BLACK);
    short pair2 = palette.getColorPair(COLOR_GREEN, COLOR_WHITE);
    assert(pair1 != pair2 && "Different color combinations should return different indices");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPairPalette_multiple_pairs_increment_indices) {
    NCURSES_SETUP;
    TColorPairPalette palette;
    short p1 = palette.getColorPair(COLOR_RED, COLOR_BLACK);
    short p2 = palette.getColorPair(COLOR_GREEN, COLOR_BLACK);
    short p3 = palette.getColorPair(COLOR_BLUE, COLOR_WHITE);
    assert(p1 > 0 && "First pair should be positive");
    assert(p2 > 0 && "Second pair should be positive");
    assert(p3 > 0 && "Third pair should be positive");
    assert((p1 != p2) && (p1 != p3) && (p2 != p3) && "All pairs should be unique");
    NCURSES_TEARDOWN;
}

#endif