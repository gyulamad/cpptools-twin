#pragma once

class TColorPair {
protected:
    // RGB values as 0xRRGGBB integers (or -1 for default/transparent).
    int fgRgb, bgRgb;
public:
    TColorPair(int fgRgb, int bgRgb): fgRgb(fgRgb), bgRgb(bgRgb) {}
    virtual ~TColorPair() {}

    bool operator==(const TColorPair& other) const {
        return fgRgb == other.fgRgb && bgRgb == other.bgRgb;
    }
};
