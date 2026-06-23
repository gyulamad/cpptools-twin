// Build and run: ./builder editor.cpp --mode=strict [--run --pty]
// Usage: ./editor <filename>  (or leave empty for a new untitled document)

// To test CTRL+S use IXON:
// stty -ixon && .editor <filename>

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


class EditorApp: public TWindow {
protected:
    string filename;
    bool modified = false;
    TInput* editor = nullptr;
    TScrollbar* vsb = nullptr;
    TScrollbar* hsb = nullptr;
    TBox* status = nullptr;
    TBox* savebtn = nullptr;
    TBox* wrapbtn = nullptr;

public:
    EditorApp(const string& filename): TWindow(), filename(filename) {
        if (filename.empty() || !file_exists(filename))
            throw ERROR("File not found");

        short cEditor = getTheme()->getInputColorPair();
        short cScrollbar = getTheme()->getScrollbarColorPair();
        short cStatus = getTheme()->getBoxColorPair();
        short cBox = getTheme()->getBoxColorPair();

        savebtn = new TBox(getRoot(), 8, 1, 0, 0, cBox, "  Save");
        savebtn->onMouseClickSubscribe([&](int, int, unsigned int, unsigned int) -> TEventResult {
            saveFile();
            return TEventResult::Handled;
        });

        wrapbtn = new TBox(getRoot(), 8, 1, 0, 10, cBox, "[?] Wrap");
        wrapbtn->onMouseClickSubscribe([&](int, int, unsigned int, unsigned int) -> TEventResult {
            bool wrap = !editor->isWrapText();
            editor->setWrapText(wrap);            
            updateStatus();
            return TEventResult::Handled;
        });

        // Editor fills the full window
        editor = new TInput(getRoot(), COLS - 1, LINES - 2, 1, 0, cEditor);
        editor->setScrollable(true);
        // editor->setWrapText(true);

        // Scrollbars attached to the right edge of the editor
        vsb = new TScrollbar(editor, cScrollbar, TOrientation::VERTICAL);
        // hsb = new TScrollbar(editor, cScrollbar, TScrollbar::HORIZONTAL);
        hsb = new TScrollbar(getRoot(), editor, 20, 1, LINES - 1, COLS - 20, cScrollbar, TOrientation::HORIZONTAL);

        status = new TBox(getRoot(), COLS - 20, 1, LINES - 1, 0, cStatus);

        setFocus(editor);

        // Load file contents if a path was given and file exists
        loadFile(filename);

        updateStatus();
        loop();
    }

    virtual ~EditorApp() override {
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
            updateStatus();
        } catch (exception& e) {
            status->setContents(e.what());
        }
    }

    void saveFile() {
        try {
            file_put_contents(filename, editor->getContents(), false, true);
            modified = false;
            updateStatus();
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
        
        updateStatus();
        return result;
    }

    TEventResult onResize(int cols, int rows) override {
        TEventResult result = TWindow::onResize(cols, rows);
        if (result & TEventResult::Stop) return result;
        if (editor) editor->setSize(cols, rows);
        return result | TEventResult::Handled;
    }

protected:
    void updateStatus() {
        bool wrap = editor->isWrapText();
        status->setContents(
            filename + ":" + 
            to_string(editor->getCursorRow()) + ":" + 
            to_string(editor->getCursorCol()) + 
            " (" + (modified ? "modified" : "saved") + ")" +
            " Wrap text: " + (wrap ? "ON" : "OFF")
        );
        wrapbtn->setContents(string("[") + (wrap ? "x" : " ") + "] Wrap");
    }
};


int main(int argc, char* argv[]) {
    Arguments args(argc, argv);
    args.addHelp(1, "file", "Path to the file to open");
    string filename = args.get<string>(1);

    EditorApp app(filename);

    return 0;
}

