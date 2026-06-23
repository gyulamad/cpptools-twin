// Build and run: ./builder colorpick.cpp --mode=strict [--run --pty]

// DEPENDENCY: ncurses/ncurses

#include "../../twin/TWindow.hpp"
#include "../../twin/TColorPicker.hpp"

// ============================================================================
// Color Picker Demo — true-color RGB picker with scrollbars
// ============================================================================

int main() {
    TWindow twin;
    TTheme* theme = twin.getTheme();
    short cTitle     = theme->getPalette().getColorPair(0xFFFFFF, 0x000000);
    short cLabelR    = theme->getPalette().getColorPair(0xFF4444, -1);
    short cLabelG    = theme->getPalette().getColorPair(0x44FF44, -1);
    short cLabelB    = theme->getPalette().getColorPair(0x4444FF, -1);
    short cSb        = theme->getPalette().getColorPair(0xFFFFFF, 0x333333);

    TBox title(twin.getRoot(), 50, 1, 0, 2, cTitle, "True-Color RGB Picker — scroll the bars");
    (void)title; // suppress unused warning

    int startX = 6;  // left column for first channel group
    int pickerW = 35; // total width of color picker widget

    TColorPicker picker(twin.getRoot(), pickerW, 4, 2, startX, cLabelR, cLabelG, cLabelB, cSb, cTitle, theme->getPalette());
    
    twin.loop();
}