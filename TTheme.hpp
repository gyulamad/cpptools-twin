#pragma once

// DEPENDENCY: ncurses/ncurses

#include <ncursesw/ncurses.h>

#include "TColorPairPalette.hpp"

class TTheme: public TColorPairPalette {
protected:
    TColorPairPalette palette = TColorPairPalette();
public:
    short getColorPair(short colorIndex, short bgColorIndex) {
        return palette.getColorPair(colorIndex, bgColorIndex);
    }

    TColorPairPalette& getPalette() { return palette; }

    virtual short getWindowColorPair() { return getColorPair(COLOR_WHITE, COLOR_BLACK); }
    virtual short getBoxColorPair() { return getColorPair(COLOR_BLACK, COLOR_GREEN); }
    virtual short getScrollbarColorPair() { return getColorPair(COLOR_WHITE, COLOR_BLUE); }
    virtual short getInputColorPair() { return getColorPair(COLOR_WHITE, COLOR_BLACK); }

    // TODO: implement the rest of getters here...
};
