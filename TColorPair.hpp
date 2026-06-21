#pragma once

class TColorPair {
protected:
    short colorIndex, bgColorIndex;
public:
    TColorPair(short colorIndex, short bgColorIndex): colorIndex(colorIndex), bgColorIndex(bgColorIndex) {}
    virtual ~TColorPair() {}
    bool operator==(const TColorPair& other) {
        return colorIndex == other.colorIndex && bgColorIndex == other.bgColorIndex;
    }
};
