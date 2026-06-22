// Build and run: ./builder twin_test.cpp --mode=strict [--run --pty]
// Optionally add '--run --pty' arguments to run after build.

// DEPENDENCY: ncurses/ncurses

#include <iostream>
#include <ostream>
#include <string>
#include <ncursesw/ncurses.h>

#include "../../misc/ERROR.hpp"
#include "../../misc/Arguments.hpp"

#include "../../twin/TWindow.hpp"
#include "../../twin/TScrollbar.hpp"
#include "../../twin/TInput.hpp"

using namespace std;


/* TODO:
#include <iostream>
#include <string>

// Base64 encode (needed for OSC 52)
std::string base64Encode(const std::string& input) {
    static const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, bits = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        bits += 8;
        while (bits >= 0) {
            out += table[(val >> bits) & 0x3F];
            bits -= 6;
        }
    }
    if (bits > -6) out += table[((val << 8) >> (bits + 8)) & 0x3F];
    while (out.size() % 4) out += '=';
    return out;
}

void toClipboard(const std::string& text) {
    // Write OSC 52 sequence directly to the terminal
    std::cout << "\033]52;c;" << base64Encode(text) << "\a" << std::flush;
}
*/




// ------------------------------------------------------
// TESTS ONLY
// ------------------------------------------------------

class TWindowTester: public TWindow {
protected:
    int curY = 0;

public:
    using TWindow::TWindow;

    virtual ~TWindowTester() {}

    TEventResult onKeyPress(int key, string name) override {
        TEventResult result = TWindow::onKeyPress(key, name);
        if (result & TEventResult::Stop) return result;
        printEvent("Key: " + name + " (" + to_string(key) + ")");
        return TEventResult::Handled | TEventResult::Stop;
    }

    TEventResult onMouseMove(int x, int y) override {
        TEventResult result = TWindow::onMouseMove(x, y);
        if (result & TEventResult::Stop) return result;
        printEvent("Mouse move: (" + to_string(x) + ", " + to_string(y) + ")");
        return TEventResult::Handled | TEventResult::Stop;
    }

    TEventResult onMouseClick(int x, int y, unsigned int button, unsigned int repeat) override {
        TEventResult result = TWindow::onMouseClick(x, y, button, repeat);
        if (result & TEventResult::Stop) return result;
        printEvent("Click button(" + to_string(button) + ") x" + to_string(repeat) + " times at (" + to_string(x) + ", " + to_string(y) + ")");
        return TEventResult::Handled | TEventResult::Stop;
    }

    TEventResult onMouseDown(int x, int y, unsigned int button) override {
        TEventResult result = TWindow::onMouseDown(x, y, button);
        if (result & TEventResult::Stop) return result;
        printEvent("MouseDown " + to_string(button) + " at (" + to_string(x) + ", " + to_string(y) + ")");
        return TEventResult::Handled | TEventResult::Stop;
    }

    TEventResult onMouseUp(int x, int y, unsigned int button) override {
        TEventResult result = TWindow::onMouseUp(x, y, button);
        if (result & TEventResult::Stop) return result;
        printEvent("MouseUp " + to_string(button) + " at (" + to_string(x) + ", " + to_string(y) + ")");
        return TEventResult::Handled | TEventResult::Stop;
    }

    TEventResult onMouseScroll(int x, int y, unsigned int direction) override {
        // Let TWindow route to the appropriate box first
        TEventResult result = TWindow::onMouseScroll(x, y, direction);
        if (result & TEventResult::Stop) return result;
        printEvent("Scroll " + to_string(direction) + " at (" + to_string(x) + ", " + to_string(y) + ")");
        return TEventResult::Handled | TEventResult::Stop;
    }

    TEventResult onResize(int cols, int rows) override {
        TEventResult result = TWindow::onResize(cols, rows);
        if (result & TEventResult::Stop) return result;
        printEvent("Resize: " + to_string(cols) + "x" + to_string(rows));
        return TEventResult::Handled | TEventResult::Stop;
    }

    void printEvent(const string& msg) {
        (void)msg; // suppress unused-parameter warning
        // root->setContents(root->getContents() + "\n" + msg);
        // mvprintw(curY, 0, "%s", msg.c_str());
        // curY++;
        // getmaxyx(stdscr, LINES, COLS);
        // if (curY >= LINES) {
        //     curY = 0;
        //     clear();
        // }
        // refresh();
    }
};


int main_test1() {
    TWindowTester twin;
    twin.getRoot()->setScrollable(true);
    twin.getRoot()->setAutoScroll(true);
    
    // // short c0 = twin.getPalette().getColorPair(COLOR_WHITE, COLOR_BLACK);
    // short cBox1 = twin.getTheme().getBoxColorPair(); // .getPalette().getColorPair(COLOR_BLACK, COLOR_GREEN);
    // short cBox2 = twin.getTheme().getPalette().getColorPair(COLOR_BLUE, COLOR_CYAN);
    // short cText = twin.getTheme().getPalette().getColorPair(COLOR_WHITE, COLOR_CYAN);
    // short cScrollbar = twin.getTheme().getScrollbarColorPair(); //.getPalette().getColorPair(COLOR_WHITE, COLOR_BLUE);
    
    short cBox1 = twin.getTheme()->getPalette().getColorPair(COLOR_BLACK, COLOR_GREEN);
    short cBox2 = twin.getTheme()->getPalette().getColorPair(COLOR_BLUE, COLOR_CYAN);
    short cText = twin.getTheme()->getPalette().getColorPair(COLOR_WHITE, COLOR_CYAN);
    short cScrollbar = twin.getTheme()->getPalette().getColorPair(COLOR_WHITE, COLOR_BLUE);

    
    // Example to chain boxes at constructor

    TBox box2(twin.getRoot(), 30, 5, 2, 35, cBox2, "BOX-2");
    TBox box1(&box2, 20, 10, 1, 1, cBox1, "BOX-1:"
        "line 1\nline 2\nline 3\nline 4\nline 5\n"
        "line 6\nline 7\nline 8\nline 9\nline 10\n"
        "line 11\nline 12\nline 13\nline 14\nline 15");
    box1.setScrollable(true);
    box2.setScrollable(true);
    box1.setScrollSpeed(2);



    // Example to chain boxes with addChild() method

    TBox box3(20, 10, 1, 1, cBox1, "BOX-3"
        "line 1\nline 2\nline 3\nline 4\nline 5\n"
        "line 6\nline 7\nline 8\nline 9\nline 10\n"
        "line 11\nline 12\nline 13\nline 14\nline 15");
    TBox box4(30, 5, 2, 75, cBox2, "BOX-4");
    box3.setScrollable(true);
    box4.setScrollable(true);
    box3.setScrollSpeed(2);

    box4.addChild(&box3);
    twin.getRoot()->addChild(&box4);

    TBox text(twin.getRoot(), 30, 5, 10, 35, cText, "This is a very long text and I want to know if it breaks lines properly. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam nunc massa, congue in vulputate ac, vestibulum vel mauris. Nunc vel aliquet ipsum. Pellentesque bibendum lacus vel enim sollicitudin, nec egestas sem lobortis. Pellentesque finibus tortor id eros eleifend, quis mollis lorem pulvinar. Vestibulum semper justo odio, quis pharetra orci fringilla sit amet. Maecenas id accumsan risus. Fusce vel sem eget est convallis congue ut eu ante. Nunc luctus vitae metus sed bibendum.");
    text.setScrollable(true);
    text.setWrapText(true);

    // Vertical scrollbar on the right edge of box1, same height
    TScrollbar vsb1(
        twin.getRoot(),                          // same parent as target box
        // text.getTopAbsolute(),          // top  — match target box
        // text.getLeftAbsolute() + 35,    // left — one col right of target box (width=20)
        // text.getHeight(),               // same height as target box
        &text,
        cScrollbar,
        TScrollbar::VERTICAL
    );
    
    TScrollbar hsb1(
        &text,
        cScrollbar,
        TScrollbar::HORIZONTAL
    );


    twin.loop();

    cout << "exited" << endl;
    return 0;
}



int main_test2() {
    TWindow twin;
    short cInput = twin.getTheme()->getInputColorPair(); //.getPalette().getColorPair(COLOR_BLACK, COLOR_WHITE);

    TInput input1(30, 5, 1, 2, cInput);
    input1.setContents("This is the default contents here.\nThis default content has multiple lines.\nThree specifically.");
    input1.setWrapText(true);
    // input1.setMultiLine(false);

    twin.getRoot()->addChild(&input1);
    twin.loop();
    cout << "exited" << endl;
    return 0;
}

int main(int argc, char** argv) {
    Arguments args(argc, argv);
    args.addHelp("func", "Add a test/example function");
    string func = args.getopt<string>("func", "main_test1");
    if (func == "main_test1") return main_test1();
    if (func == "main_test2") return main_test2();
    throw ERROR("--func argument is missing or invalid.");
}
