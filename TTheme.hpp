#pragma once

// DEPENDENCY: ncurses/ncurses

#include <ncursesw/ncurses.h>

#include "TColorPairPalette.hpp"

class TTheme: public TColorPairPalette {
protected:
    // True-color RGB constants (0xRRGGBB). -1 = default/transparent.
    static constexpr int C_BLACK   = 0x000000;
    static constexpr int C_WHITE   = 0xFFFFFF;
    static constexpr int C_RED     = 0xFF0000;
    static constexpr int C_GREEN   = 0x00FF00;
    static constexpr int C_YELLOW  = 0xFFFF00;
    static constexpr int C_BLUE    = 0x0000FF;
    static constexpr int C_MAGENTA = 0xFF00FF;
    static constexpr int C_CYAN    = 0x00FFFF;

    TColorPairPalette palette = TColorPairPalette();
public:
    short getColorPair(int fgRgb, int bgRgb) {
        return palette.getColorPair(fgRgb, bgRgb);
    }

    TColorPairPalette& getPalette() { return palette; }

    virtual short getWindowColorPair()   { return getColorPair(C_WHITE, C_BLACK); }
    virtual short getBoxColorPair()      { return getColorPair(C_BLACK, C_GREEN); }
    virtual short getScrollbarColorPair(){ return getColorPair(C_WHITE, C_BLUE); }
    virtual short getInputColorPair()    { return getColorPair(C_BLACK, C_WHITE); }

    // TODO: implement the rest of getters here...
};
