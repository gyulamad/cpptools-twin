// Build and run: ./builder notepad.cpp --mode=strict [--run --pty]
// Usage: ./notepad <filename>  (or leave empty for a new untitled document)

// To test CTRL+S use IXON:
// stty -ixon && .notepad <filename>

// DEPENDENCY: ncurses/ncurses

#include <exception>
#include <string>
#include <ncursesw/ncurses.h>

#include "../../misc/Arguments.hpp"
#include "../../misc/file_get_contents.hpp"
#include "../../misc/file_put_contents.hpp"
#include "../../misc/file_exists.hpp"

#include "../TWindow.hpp"
#include "../TInput.hpp"
#include "../TScrollbar.hpp"

using namespace std;


class NotepadApp: public TWindow {
protected:
    string filename;
    bool modified = false;
    TInput* editor = nullptr;
    TScrollbar* vsb = nullptr;
    TScrollbar* hsb = nullptr;
    TBox* status = nullptr;

public:
    NotepadApp(const string& filename): TWindow(), filename(filename) {
        if (filename.empty() || !file_exists(filename))
            throw ERROR("File not found");

        short cEditor = getTheme()->getInputColorPair();
        short cScrollbar = getTheme()->getScrollbarColorPair();
        short cStatus = getTheme()->getWindowColorPair();

        // Editor fills the full window
        editor = new TInput(getRoot(), COLS - 1, LINES - 2, 0, 0, cEditor);
        editor->setScrollable(true);

        // Scrollbars attached to the right edge of the editor
        vsb = new TScrollbar(editor, cScrollbar, TScrollbar::VERTICAL);
        hsb = new TScrollbar(editor, cScrollbar, TScrollbar::HORIZONTAL);

        status = new TBox(getRoot(), COLS, 1, LINES - 1, 0, cStatus);

        setFocus(editor);

        // Load file contents if a path was given and file exists
        loadFile(filename);

        loop();
    }

    virtual ~NotepadApp() override {
        delete status;
        delete vsb;
        delete hsb;
        delete editor;
    }

protected:
    void loadFile(const string& path) {
        try {
            string content = file_get_contents(path);
            editor->setContents(content);
            filename = path;
            modified = false;
            showStatus();
        } catch (exception& e) {
            status->setContents(e.what());
        }
    }

    void saveFile() {
        try {
            file_put_contents(filename, editor->getContents(), false, true);
            modified = false;
            showStatus();
        } catch (exception& e) {
            status->setContents(e.what());
        }
    }

   TEventResult onKeyPress(int key, string name) override {
        TEventResult result = TWindow::onKeyPress(key, name);
        if (result & TEventResult::Stop) return result;

        // Ctrl+S — save
        if (key == 19) {
            saveFile();
            return result | TEventResult::Handled;
        }

        // Ctrl+Q or Ctrl+C — quit
        if (key == 17 || key == 3) {
            exit(0);
        }

        modified = true;
        
        showStatus();
        return result;
    }

    TEventResult onResize(int cols, int rows) override {
        TEventResult result = TWindow::onResize(cols, rows);
        if (result & TEventResult::Stop) return result;
        if (editor) editor->setSize(cols, rows);
        return result | TEventResult::Handled;
    }

protected:
    void showStatus() {
        status->setContents(
            filename + ":" + 
            to_string(editor->getCursorCol()) + ":" + 
            to_string(editor->getCursorRow()) + 
            " (" + (modified ? "modified" : "saved") + ")"
        );
    }
};


int main(int argc, char* argv[]) {
    Arguments args(argc, argv);
    args.addHelp(1, "file", "Path to the file to open");
    string filename = args.get<string>(1);

    NotepadApp app(filename);

    return 0;
}

