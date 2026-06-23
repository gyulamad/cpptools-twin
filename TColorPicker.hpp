#pragma once

// DEPENDENCY: ncurses/ncurses

#include <string>
#include <vector>
#include "TBox.hpp"
#include "TScrollbar.hpp"
#include "TColorPairPalette.hpp"
#include "../misc/hex.hpp"

using namespace std;

class TColorChannel : public ITScrollable {
protected:
    int value = 128;
public:
    ~TColorChannel() override {}

    void setValue(int v) {
        value = max(0, min(v, 255));
        notifyScrollChange();
    }
    int getValue() const { return value; }

    // ITScrollable interface — orientation is ignored (single-axis).
    int getScrollValue(int /*orientation*/) const override { return value; }
    int getScrollMin(int /*orientation*/)   const override { return 0; }
    int getScrollMax(int /*orientation*/)   const override { return 255; }

    void setScrollValue(int v, int /*orientation*/) override { setValue(v); }
};

class TColorPicker : public TBox {
public:
    // Creates a color picker with R/G/B horizontal scrollbars and preview.
    // width     - total widget width (scrollbarWidth + label/gap/value padding)
    // height    - total widget height (minimum 4: 3 scrollbar rows + 1 preview row)
    // top, left - position relative to parent
    TColorPicker(TBox* parent, int width, int height, int top, int left,
                 short cLabelR, short cLabelG, short cLabelB,
                 short cSb, short cPreview,
                 TColorPairPalette& palette,
                 int initialR = 128, int initialG = 128, int initialB = 128)
        : TBox(parent, width, max(height, 4), top, left, cPreview), m_palette(palette)
    {
        // Layout constants: label(2) + gap(1) + scrollbar(?) + gap(2) + value(4) = totalWidth
        int channelLabelW = 2;
        int sbGapBefore   = 1;
        int valGapAfter   = 2;
        int valW          = 4;

        // Derive scrollbar width from picker width.
        int sbWidth = width - channelLabelW - sbGapBefore - valGapAfter - valW;
        if (sbWidth < 1) {
            throw ERROR("TColorPicker: width too small for layout");
        }

        int totalH = max(height, 4);
        int previewH = totalH - 3; // remaining rows after scrollbars
        string hexC = "#" + uint2hex(((unsigned int)initialR << 16) | ((unsigned int)initialG << 8) | (unsigned int)initialB);

        chR.setValue(initialR);
        chG.setValue(initialG);
        chB.setValue(initialB);

        auto makeLabel = [&](const string& txt) -> vector<string> { return {txt}; };

        // --- Channel labels, scrollbars, and value boxes (rows 0-2 relative) ---
        TBox* lblR = new TBox(this, channelLabelW, 1, 0, 0, cLabelR, makeLabel("R"));
        lblR->setPosition(0, 0);
        
        sbR = new TScrollbar(this, sbWidth, 1, 0, channelLabelW + sbGapBefore, cSb, TScrollbar::HORIZONTAL);
        
        valR = new TBox(this, valW, 1, 0, channelLabelW + sbGapBefore + sbWidth + valGapAfter, cPreview, makeLabel(to_string(initialR)));
        valR->setPosition(0, channelLabelW + sbGapBefore + sbWidth + valGapAfter);

        TBox* lblG = new TBox(this, channelLabelW, 1, 0, 0, cLabelG, makeLabel("G"));
        lblG->setPosition(1, 0);
        
        sbG = new TScrollbar(this, sbWidth, 1, 1, channelLabelW + sbGapBefore, cSb, TScrollbar::HORIZONTAL);
        
        valG = new TBox(this, valW, 1, 0, channelLabelW + sbGapBefore + sbWidth + valGapAfter, cPreview, makeLabel(to_string(initialG)));
        valG->setPosition(1, channelLabelW + sbGapBefore + sbWidth + valGapAfter);

        TBox* lblB = new TBox(this, channelLabelW, 1, 0, 0, cLabelB, makeLabel("B"));
        lblB->setPosition(2, 0);
        
        sbB = new TScrollbar(this, sbWidth, 1, 2, channelLabelW + sbGapBefore, cSb, TScrollbar::HORIZONTAL);
        
        valB = new TBox(this, valW, 1, 0, channelLabelW + sbGapBefore + sbWidth + valGapAfter, cPreview, makeLabel(to_string(initialB)));
        valB->setPosition(2, channelLabelW + sbGapBefore + sbWidth + valGapAfter);

        // --- Preview box (below scrollbars) ---
        preview = new TBox(this, sbWidth, previewH, 3, channelLabelW + sbGapBefore, cPreview, makeLabel(hexC));

        // Wire scrollbars to channels with fast scroll speed.
        sbR->setTarget(&chR);
        sbG->setTarget(&chG);
        sbB->setTarget(&chB);
        sbR->setScrollSpeed(10);
        sbG->setScrollSpeed(10);
        sbB->setScrollSpeed(10);

        // Subscribe to channel changes for preview update.
        chR.onScrollChangeSubscribe([this]() { updatePreview(); });
        chG.onScrollChangeSubscribe([this]() { updatePreview(); });
        chB.onScrollChangeSubscribe([this]() { updatePreview(); });
    }

    ~TColorPicker() override {
        delete preview;
        delete valB;
        delete valG;
        delete valR;
        delete sbB;
        delete sbG;
        delete sbR;
    }

    TColorChannel& getChannelR() { return chR; }
    TColorChannel& getChannelG() { return chG; }
    TColorChannel& getChannelB() { return chB; }

private:
TColorPairPalette& m_palette;
TColorChannel chR, chG, chB;
    TScrollbar* sbR = nullptr;
    TScrollbar* sbG = nullptr;
    TScrollbar* sbB = nullptr;
    TBox* valR      = nullptr;
    TBox* valG      = nullptr;
    TBox* valB      = nullptr;
    TBox* preview   = nullptr;

    void updatePreview() {
        int r = chR.getValue(), g = chG.getValue(), b = chB.getValue();
        
        auto makeLabel = [&](const string& txt) -> vector<string> { return {txt}; };

        if (valR) valR->setContents(makeLabel(to_string(r)));
        if (valG) valG->setContents(makeLabel(to_string(g)));
        if (valB) valB->setContents(makeLabel(to_string(b)));

        unsigned int rgb = ((unsigned int)r << 16) | ((unsigned int)g << 8) | (unsigned int)b;
        short cp = m_palette.getColorPair((int)rgb, -1);

        preview->setColorPair(cp);
        string hexColor = "#" + uint2hex(rgb);
        preview->setContents(makeLabel(hexColor));
    }
};
