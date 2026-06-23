#pragma once

// DEPENDENCY: ncurses/ncurses

#include <vector>
#include <ncursesw/ncurses.h>

#include "../misc/array_search.hpp"
#include "../misc/ERROR.hpp"

#include "TColorPair.hpp"

using namespace std;

class TColorPairPalette {
protected:
    vector<TColorPair> colorPairs;
public:
    TColorPairPalette() {}
    virtual ~TColorPairPalette() {}

    // Returns a ncurses COLOR_PAIR index for the given RGB colors.
    // fgRgb / bgRgb are 0xRRGGBB values (or -1 for default/transparent).
    // Falls back to init_pair() when true color is not available, mapping
    // known RGB constants back to their ncurses palette indices.
    short getColorPair(int fgRgb, int bgRgb) {
        TColorPair colorPair(fgRgb, bgRgb);
        auto colorPairIndex = array_search(colorPair, colorPairs);
        if (ERR != colorPairIndex) return colorPairIndex + 1;
        int pairNum = static_cast<int>(colorPairs.size()) + 1;
        // Try true-color first. Fall back to standard init_pair for older terminals.
        if (init_extended_pair(pairNum, fgRgb, bgRgb) == ERR) {
            short mappedFg = mapRgbToIndex(fgRgb);
            short mappedBg = mapRgbToIndex(bgRgb);
            if (init_pair(pairNum, mappedFg, mappedBg) == ERR)
                throw ERROR("Unable to initialize color pair");
        }
        colorPairs.push_back(colorPair);
        return static_cast<short>(pairNum);
    }

private:
    // Maps common 0xRRGGBB constants back to ncurses COLOR_* indices.
    // Returns -1 (default) for unrecognized values — the caller should not
    // reach this path on true-color terminals, but it provides a safe fallback.
    short mapRgbToIndex(int rgb) {
        if (rgb == -1) return -1;
        switch (rgb & 0xFFFFFF) {
            case 0x000000: return COLOR_BLACK;   // 0
            case 0xFF0000: return COLOR_RED;     // 1
            case 0x00FF00: return COLOR_GREEN;   // 2
            case 0xFFFF00: return COLOR_YELLOW;  // 3
            case 0x0000FF: return COLOR_BLUE;    // 4
            case 0xFF00FF: return COLOR_MAGENTA; // 5
            case 0x00FFFF: return COLOR_CYAN;    // 6
            case 0xFFFFFF: return COLOR_WHITE;   // 7
            default:       return -1;            // unrecognized — use terminal default
        }
    }
};
