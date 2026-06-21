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

    short getColorPair(short colorIndex, short bgColorIndex) {
        TColorPair colorPair(colorIndex, bgColorIndex);
        auto colorPairIndex = array_search(colorPair, colorPairs);
        if (ERR != colorPairIndex) return colorPairIndex;
        if (ERR == init_pair(colorPairs.size() + 1, colorIndex, bgColorIndex))
            throw ERROR("Unable to initialize color pair");
        colorPairs.push_back(colorPair);
        return colorPairs.size();
    }
};
