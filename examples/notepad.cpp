// Build and run: ./builder notepad.cpp --mode=strict [--run --pty]
// Usage: ./notepad --file=<path>  (or leave empty for a new untitled document)

// DEPENDENCY: ncurses/ncurses

#include <string>
#include <ncursesw/ncurses.h>

#include "../../misc/Arguments.hpp"
#include "../../misc/file_get_contents.hpp"
#include "../../misc/file_put_contents.hpp"

#include "../TWindow.hpp"
#include "../TInput.hpp"
#include "../TScrollbar.hpp"

using namespace std;


class NotepadApp: public TWindow {
protected:
    string filePath_;
    bool   modified_ = false;
    TInput* editor_  = nullptr;

public:
    NotepadApp(const string& filePath) : filePath_(filePath) {
        short cEditor = getTheme()->getInputColorPair();
        short cScrollbar = getTheme()->getScrollbarColorPair();

        // Editor fills the full window
        editor_ = new TInput(getRoot(), COLS, LINES, 0, 0, cEditor);
        editor_->setScrollable(true);

        // Vertical scrollbar attached to the right edge of the editor
        auto* vsb = new TScrollbar(editor_, cScrollbar, TScrollbar::VERTICAL);
        getRoot()->addChild(vsb);

        setFocus(editor_);

        // Load file contents if a path was given and file exists
        if (!filePath_.empty()) {
            loadFile(filePath_);
        }
    }

    ~NotepadApp() override {
        delete editor_;
    }

protected:
    void loadFile(const string& path) {
        try {
            string content = file_get_contents(path);
            editor_->setContents(content);
            filePath_ = path;
            modified_ = false;
        } catch (const runtime_error&) {
            // File does not exist yet — start with an empty editor.
            filePath_ = path;
        }
    }

    void saveFile() {
        if (filePath_.empty()) return;
        file_put_contents(filePath_, editor_->getContents(), false, true);
        modified_ = false;
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

        return result;
    }

    TEventResult onResize(int cols, int rows) override {
        TEventResult result = TWindow::onResize(cols, rows);
        if (result & TEventResult::Stop) return result;
        if (editor_) editor_->setSize(cols, rows);
        return result | TEventResult::Handled;
    }
};


int main(int argc, char* argv[]) {
    Arguments args(argc, argv);
    args.addHelp("file", "Path to the file to open");
    string filePath = args.getopt<string>("file", "");

    NotepadApp app(filePath);
    app.loop();

    return 0;
}

