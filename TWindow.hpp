#pragma once

#include <termios.h>
#include <unistd.h>

#include "TEventHandler.hpp"
#include "TBox.hpp"
#include "TTheme.hpp"

class TWindow: public TEventHandler {
protected:
    TTheme* theme = nullptr;
    bool ownTheme = false;
//    TColorPairPalette palette;
    TBox* root = nullptr;
    TEventHandler* focus = nullptr;

public:
    TWindow(TTheme* theme = nullptr): theme(theme) {
        setlocale(LC_ALL, ""); // <- Unicode fix?!
        initscr();

        // enable ctrl+s
        struct termios tty;
        tcgetattr(STDIN_FILENO, &tty);
        tty.c_iflag &= ~IXON;
        tcsetattr(STDIN_FILENO, TCSANOW, &tty);

        start_color();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);
        timeout(100);
        mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

        if (!theme) {
            ownTheme = true;
            this->theme = new TTheme();
        }

        short colorPair = this->theme->getWindowColorPair();
        root = new TBox(colorPair);
        root->setSize(COLS, LINES);
    }

    virtual ~TWindow() {        
        delete root;
        if (ownTheme) delete theme;
        endwin();
    }

    TTheme* getTheme() { return theme; }
    TBox* getRoot() { return root; }


    void setFocus(TBox* box) {
        if (focus == box) return;
        if (focus) {
            focus->setFocused(false);
            focus->onFocusLeave();
        }
        focus = box;
        if (focus) {
            focus->setFocused(true);
            focus->onFocusGain();
        }
    }

    TEventHandler* getFocus() const {
        return focus;
    }
    

    void loop() {
        while (true) {
            root->draw();
            refresh();

            int event = getch();
            if (event == ERR) continue;
            if (event == KEY_MOUSE) {
                handleMouse();
                flushinp();
            } else if (event == KEY_RESIZE) {
                // KEY_RESIZE already updated LINES/COLS internally in ncurses
                int rows = LINES;
                int cols = COLS;
                root->setSize(COLS, LINES);
                onResize(rows, cols);
            } else {
                onKeyPress(event, keyName(event));
            }
        }
    }

protected:

    TEventResult onKeyPress(int key, string name) override {
        TEventResult result = TEventHandler::onKeyPress(key, name);
        if (result & TEventResult::Stop) return result;
        if (focus == nullptr) return result;
        return focus->onKeyPress(key, name);
    }

    TEventResult onMouseMove(int x, int y) override {
        TEventResult result = TEventHandler::onMouseMove(x, y);
        if (result & TEventResult::Stop) return result;
        if (focus == nullptr) return result;
        return focus->onMouseMove(x, y);
    }

    TEventResult onMouseClick(int x, int y, unsigned int button, unsigned int repeat) override {
        TEventResult result = TEventHandler::onMouseClick(x, y, button, repeat);
        if (result & TEventResult::Stop) return result;
        TBox* target = root->findMouseAt(x, y);
        if (target == nullptr) return result;
        setFocus(target);
        return target->onMouseClick(x, y, button, repeat);
    }

    TEventResult onMouseDown(int x, int y, unsigned int button) override {
        TEventResult result = TEventHandler::onMouseDown(x, y, button);
        if (result & TEventResult::Stop) return result;
        TBox* target = root->findMouseAt(x, y);
        if (target == nullptr) return result;
        setFocus(target);
        return target->onMouseDown(x, y, button);
    }

    TEventResult onMouseUp(int x, int y, unsigned int button) override {
        TEventResult result = TEventHandler::onMouseUp(x, y, button);
        if (result & TEventResult::Stop) return result;
        TBox* target = root->findMouseAt(x, y);
        if (target == nullptr) return result;
        setFocus(target);
        return target->onMouseUp(x, y, button);
    }

    TEventResult onMouseScroll(int x, int y, unsigned int direction) override {
        TEventResult result = TEventHandler::onMouseScroll(x, y, direction);
        if (result & TEventResult::Stop) return result;
        // Route the scroll event to the deepest scrollable box under the
        // cursor. Focus is intentionally NOT changed by scroll events.
        TBox* target = root->findScrollableAt(x, y);
        if (target) return target->onMouseScroll(x, y, direction);
        return result;
    }

    TEventResult onResize(int cols, int rows) override {
        TEventResult result = TEventHandler::onResize(cols, rows);
        if (result & TEventResult::Stop) return result;
        root->setSize(cols, rows);
        return root->onResize(cols, rows);
    }

private:

    void handleMouse() {
        MEVENT mevent;
        if (getmouse(&mevent) != OK) return;

        if (isClick(mevent.bstate)) {
            int button = -1;
            int repeat = -1;

            if (mevent.bstate & BUTTON1_CLICKED) {
                button = 1; repeat = 1;
            } else if (mevent.bstate & BUTTON2_CLICKED) {
                button = 2; repeat = 1;
            } else if (mevent.bstate & BUTTON3_CLICKED) {
                button = 3; repeat = 1;
            } else if (mevent.bstate & BUTTON4_CLICKED) {
                button = 4; repeat = 1;
            } else if (mevent.bstate & BUTTON1_DOUBLE_CLICKED) {
                button = 1; repeat = 2;
            } else if (mevent.bstate & BUTTON2_DOUBLE_CLICKED) {
                button = 2; repeat = 2;
            } else if (mevent.bstate & BUTTON3_DOUBLE_CLICKED) {
                button = 3; repeat = 2;
            } else if (mevent.bstate & BUTTON4_DOUBLE_CLICKED) {
                button = 4; repeat = 2;
            } else if (mevent.bstate & BUTTON1_TRIPLE_CLICKED) {
                button = 1; repeat = 3;
            } else if (mevent.bstate & BUTTON2_TRIPLE_CLICKED) {
                button = 2; repeat = 3;
            } else if (mevent.bstate & BUTTON3_TRIPLE_CLICKED) {
                button = 3; repeat = 3;
            } else if (mevent.bstate & BUTTON4_TRIPLE_CLICKED) {
                button = 4; repeat = 3;
            }

            onMouseClick(mevent.x, mevent.y, button, repeat);
            return;
        }
        if (isPressed(mevent.bstate)) {
            int button = -1;
            if (mevent.bstate & BUTTON1_PRESSED) button = 1;
            else if (mevent.bstate & BUTTON2_PRESSED) button = 2;
            else if (mevent.bstate & BUTTON3_PRESSED) button = 3;
            onMouseDown(mevent.x, mevent.y, button);
            return;
        }
        if (isReleased(mevent.bstate)) {
            int button = -1;
            if (mevent.bstate & BUTTON1_RELEASED) button = 1;
            else if (mevent.bstate & BUTTON2_RELEASED) button = 2;
            else if (mevent.bstate & BUTTON3_RELEASED) button = 3;
            onMouseUp(mevent.x, mevent.y, button);
            return;
        }
        if (mevent.bstate & BUTTON4_PRESSED || mevent.bstate & BUTTON5_PRESSED) {    
            int direction = -1;
            if (mevent.bstate & BUTTON4_PRESSED) direction = 4;
            else if (mevent.bstate & BUTTON5_PRESSED) direction = 5;
            onMouseScroll(mevent.x, mevent.y, direction);
            return;
        }
        onMouseMove(mevent.x, mevent.y); // Note: ncurses bug or versioning issue: mouse-move never handled!
    }

    bool isClick(mmask_t bstate) {
        mmask_t clickMask = BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED | BUTTON1_TRIPLE_CLICKED
                          | BUTTON2_CLICKED | BUTTON2_DOUBLE_CLICKED | BUTTON2_TRIPLE_CLICKED
                          | BUTTON3_CLICKED | BUTTON3_DOUBLE_CLICKED | BUTTON3_TRIPLE_CLICKED;
        return (bstate & clickMask) != 0;
    }

    bool isPressed(mmask_t bstate) {
        mmask_t pressMask = BUTTON1_PRESSED | BUTTON2_PRESSED | BUTTON3_PRESSED;
        return (bstate & pressMask) != 0;
    }

    bool isReleased(mmask_t bstate) {
        mmask_t releaseMask = BUTTON1_RELEASED | BUTTON2_RELEASED | BUTTON3_RELEASED;
        return (bstate & releaseMask) != 0;
    }

    static string keyName(int key) {
        // Special keys via KEY_* macros
        if (key == KEY_UP) return "Up";
        if (key == KEY_DOWN) return "Down";
        if (key == KEY_LEFT) return "Left";
        if (key == KEY_RIGHT) return "Right";
        if (key == KEY_HOME) return "Home";
        if (key == KEY_END) return "End";
        if (key == KEY_PPAGE) return "PgUp";
        if (key == KEY_NPAGE) return "PgDn";
        if (key == KEY_DC) return "Delete";
        if (key == KEY_IC) return "Insert";
        if (key == KEY_BACKSPACE) return "Backspace";
        if (key == '\t') return "Tab";
        if (key == KEY_ENTER || key == '\n') return "Enter";
        if (key == 27) return "Esc";

        // Function keys F1-F64 via KEY_F(n) macro
        const int base = KEY_F(0);
        const int step = KEY_F(1) - KEY_F(0);
        if (key >= base && key < base + step * 64)
            return "F" + to_string((key - base) / step);

        // Control characters (values 0-31) using ^ notation
        if (key >= 0 && key < 32)
            return "^" + string(1, static_cast<char>(key + '@'));

        // Printable ASCII
        if (key >= 32 && key <= 126)
            return string(1, static_cast<char>(key));

        // Fallback for any other code
        return "(" + to_string(key) + ")";
    }
};
