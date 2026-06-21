#pragma once

#include "TBox.hpp"

class TInput: public TBox {
protected:
    int cursorRow = 0;
    int cursorCol = 0;
    bool multiLine = false;

public:
    using TBox::TBox;

    TInput(TBox* parent, short colorPair): 
        TInput(parent, 0, 0, 0, 0, colorPair) {}

    TInput(TBox* parent, int w, int h, short colorPair): 
        TInput(parent, w, h, 0, 0, colorPair) {}

    TInput(TBox* parent, int w, int h, int t, int l, short colorPair):
        TBox(parent, w, h, t, l, colorPair) {
        setMultiLine(height > 1);
    }

    TInput(short colorPair): 
        TInput(nullptr, colorPair) {}

    TInput(int w, int h, short colorPair): 
        TInput(nullptr, w, h, colorPair) {}

    TInput(int w, int h, int t, int l, short colorPair):
        TInput(nullptr, w, h, t, l, colorPair) {}

    virtual ~TInput() { curs_set(0); }

    // Setters/Getters
    void setMultiLine(bool value) { multiLine = value; }
    bool getMultiLine() const { return multiLine; }

    int getCursorRow() const { return cursorRow; }
    int getCursorCol() const { return cursorCol; }

    // Override setSize to update multiline default
    void setSize(int w, int h) override {
        TBox::setSize(w, h);
        multiLine = (h > 1);
        clampCursor();
    }

protected:
    void onFocusGain() override {
        TEventHandler::onFocusGain();
        curs_set(1);
        dirty = true;
    }

    void onFocusLeave() override {
        curs_set(0);
        TEventHandler::onFocusLeave();
        dirty = true;
    }

    bool onKeyPress(int key, string name) override {
        if (!TBox::onKeyPress(key, name)) return false;

        // Printable ASCII characters
        if (key >= 32 && key <= 126) {
            insertChar((char)key);
            return true;
        }

        if (key == KEY_BACKSPACE || key == 127) {
            handleBackspace();
            return true;
        }

        if (key == KEY_DC) {
            handleDelete();
            return true;
        }

        if (key == '\n' || key == KEY_ENTER) {
            if (multiLine) {
                insertNewline();
            }
            return true;
        }

        // Tab — batch-insert spaces for efficiency and consistent rendering.
        if (key == '\t') {
            ensureLine(cursorRow);
            auto& line = getTextLines()[cursorRow];
            if (cursorCol > (int)line.size()) cursorCol = (int)line.size();
            line.insert((size_t)cursorCol, 4, ' ');
            cursorCol += 4;
            updateContentsFromRaw();
            return true;
        }

        // Arrow keys
        if (name == "Up")    { moveCursorUp();    return true; }
        if (name == "Down")  { moveCursorDown();  return true; }
        if (name == "Left")  { moveCursorLeft();  return true; }
        if (name == "Right") { moveCursorRight(); return true; }

        // Home/End
        if (name == "Home" || name == "^A") { moveCursorHome();   return true; }
        if (name == "End"  || name == "^E") { moveCursorEnd();    return true; }

        // Ctrl+Home / Ctrl+End — start/end of document
        if (key == KEY_HOME) { moveCursorDocStart(); return true; }
        if (key == KEY_END)  { moveCursorDocEnd();   return true; }

        // Unhandled key — let parent handlers process it.
        return false;
    }

    bool onMouseClick(int x, int y, unsigned int button, unsigned int repeat) override {
        if (!TBox::onMouseClick(x, y, button, repeat)) return false;
        moveToScreenPos(x, y);
        return true;
    }

    void redraw() override {
        attron(COLOR_PAIR(colorPair));

        // Let TBox draw the underlying text first (it handles clipping and scrolling)
        TBox::redraw();

        // Position ncurses cursor if this input has focus
        if (hasFocus()) {
            int visualRow = getVisualCursorRow();
            int visualCol = wrapText ? getVisualCursorCol() : cursorCol;
            int screenRow = getTopAbsolute() + visualRow - scrollTop;
            int screenCol = getLeftAbsolute() + visualCol - scrollLeft;
            move(screenRow, screenCol);
        }

        attroff(COLOR_PAIR(colorPair));
    }

private:
    // Get the raw text lines for editing
    vector<string>& getTextLines() { return rawContents; }
    const vector<string>& getCTextLines() const { return rawContents; }

    void ensureLine(int row) {
        auto& lines = getTextLines();
        if ((int)lines.size() <= row) {
            lines.resize(row + 1, "");
            dirty = true;
            recalculateBounds();
            clampScroll();
        }
    }

    // Insert a character at cursor position
    void insertChar(char ch) {
        ensureLine(cursorRow);
        auto& line = getTextLines()[cursorRow];
        if (cursorCol > (int)line.size()) cursorCol = (int)line.size();
        line.insert((size_t)cursorCol, 1, ch);
        cursorCol++;
        updateContentsFromRaw();
    }

   // Insert a newline at cursor position (multi-line mode)
    void insertNewline() {
        auto& lines = getTextLines();
        ensureLine(cursorRow);
        string currentLine = lines[cursorRow];
        string rightPart = "";
        if (cursorCol < (int)currentLine.size()) {
            rightPart = currentLine.substr((size_t)cursorCol);
            currentLine = currentLine.substr(0, (size_t)cursorCol);
        }
        lines[cursorRow] = currentLine;
        lines.insert(lines.begin() + cursorRow + 1, rightPart);

        cursorRow++;
        cursorCol = 0;
        updateContentsFromRaw();
    }

    // Backspace: delete char before cursor or merge with previous line at start
    void handleBackspace() {
        if (cursorCol > 0) {
            // Validate cursor position before accessing.
            const auto& clines = getCTextLines();
            if (cursorRow >= (int)clines.size()) return;
            auto& lines = getTextLines();
            ensureLine(cursorRow);
            string& line = lines[cursorRow];
            line.erase((size_t)(cursorCol - 1), 1);
            cursorCol--;
            updateContentsFromRaw();
        } else if (cursorRow > 0) {
            // Merge with previous line — no need for ensureLine since we're deleting this row.
            auto& lines = getTextLines();
            string prevLine = lines[cursorRow - 1];
            string currentLine = lines[cursorRow];
            lines[cursorRow - 1] = prevLine + currentLine;

            // Remove current line and shift remaining up
            for (int i = cursorRow; i < (int)lines.size() - 1; ++i) {
                lines[i] = lines[i + 1];
            }
            lines.pop_back();

            cursorRow--;
            cursorCol = (int)prevLine.size();
            updateContentsFromRaw();
        }
    }

    // Delete: delete char at cursor or merge with next line at end
    void handleDelete() {
        auto& lines = getTextLines();
        ensureLine(cursorRow);
        string& line = lines[cursorRow];

        if (cursorCol < (int)line.size()) {
            line.erase((size_t)cursorCol, 1);
            updateContentsFromRaw();
        } else if (cursorRow + 1 < (int)lines.size()) {
            // Merge with next line
            string currentLine = lines[cursorRow];
            string nextLine = lines[cursorRow + 1];
            lines[cursorRow] = currentLine + nextLine;

            for (int i = cursorRow + 1; i < (int)lines.size() - 1; ++i) {
                lines[i] = lines[i + 1];
            }
            lines.pop_back();

            updateContentsFromRaw();
        }
    }

    // Cursor movement — operates on raw text coordinates (read-only, no mutation)
    void moveCursorUp() {
        // In single-line mode, prevent moving to previous rows.
        if (!multiLine && cursorRow == 0) return;

        const auto& lines = getCTextLines();
        if (wrapText && !lines.empty()) {
            // Navigate by visual line: convert → adjust visRow → convert back.
            int vrow = getVisualCursorRow();
            int vcol = getVisualCursorCol();
            if (vrow > 0) {
                convertVisualToRaw(vrow - 1, vcol);
                scrollToShowCursor();
                dirty = true;
            }
        } else if (cursorRow > 0) {
            cursorRow--;
            clampColToCurrentLine();
            scrollToShowCursor();
            dirty = true;
        }
    }

    void moveCursorDown() {
        const auto& lines = getCTextLines();
        // In single-line mode, never move down from row 0.
        if (!multiLine) return;

        if (wrapText && !lines.empty()) {
            // Navigate by visual line: convert → adjust visRow → convert back.
            int vrow = getVisualCursorRow();
            int vcol = getVisualCursorCol();
            const auto& visLines = contents;
            if (vrow + 1 < (int)visLines.size()) {
                // Clamp to target visual line length.
                int targetVcol = min(vcol, (int)visLines[vrow + 1].size());
                convertVisualToRaw(vrow + 1, targetVcol);
                scrollToShowCursor();
                dirty = true;
            }
        } else if (cursorRow + 1 < (int)lines.size()) {
            cursorRow++;
            clampColToCurrentLine();
            scrollToShowCursor();
            dirty = true;
        }
    }

    void moveCursorLeft() {
        // In single-line mode, prevent moving to previous rows.
        if (!multiLine && cursorRow == 0 && cursorCol == 0) return;
        if (cursorCol > 0) {
            cursorCol--;
            scrollToShowCursor();
            dirty = true;
        } else if (cursorRow > 0 && multiLine) {
            cursorRow--;
            const auto& lines = getCTextLines();
            cursorCol = (int)lines[cursorRow].size();
            scrollToShowCursor();
            dirty = true;
        }
    }

    void moveCursorRight() {
        const auto& lines = getCTextLines();
        // In single-line mode, prevent moving to next rows.
        if (!multiLine && cursorCol >= (int)lines[cursorRow].size()) return;
        if (cursorRow >= (int)lines.size()) return;
        const string& line = lines[cursorRow];
        if (cursorCol < (int)line.size()) {
            cursorCol++;
            scrollToShowCursor();
            dirty = true;
        } else if (cursorRow + 1 < (int)lines.size() && multiLine) {
            cursorRow++;
            cursorCol = 0;
            scrollToShowCursor();
            dirty = true;
        }
    }

    void moveCursorHome() {
        const auto& lines = getCTextLines();
        if (cursorRow >= (int)lines.size()) return;
        if (cursorCol != 0) {
            cursorCol = 0;
            scrollToShowCursor();
            dirty = true;
        }
    }

    void moveCursorEnd() {
        const auto& lines = getCTextLines();
        if (cursorRow >= (int)lines.size()) return;
        const string& line = lines[cursorRow];
        if (cursorCol != (int)line.size()) {
            cursorCol = (int)line.size();
            scrollToShowCursor();
            dirty = true;
        }
    }

    void moveCursorDocStart() {
        if (cursorRow != 0 || cursorCol != 0) {
            cursorRow = 0;
            cursorCol = 0;
            scrollToShowCursor();
            dirty = true;
        }
    }

   void moveCursorDocEnd() {
        const auto& lines = getCTextLines();
        if (lines.empty()) return;
        int lastRow = (int)lines.size() - 1;
        int lastCol = (int)lines[lastRow].size();
        if (cursorRow != lastRow || cursorCol != lastCol) {
            cursorRow = lastRow;
            cursorCol = lastCol;
            scrollToShowCursor();
            dirty = true;
        }
    }

    // Map screen coordinates to text position and move cursor there
    void moveToScreenPos(int x, int y) {
        int relX = x - getLeftAbsolute();
        int relY = y - getTopAbsolute();
        if (relX < 0 || relX >= width || relY < 0 || relY >= height) return;

        // Map to raw text line using wrapped contents offset by scroll
        int contentLineIdx = relY + scrollTop;
        const auto& visLines = contents;
        if (contentLineIdx < 0 || contentLineIdx >= (int)visLines.size()) return;

        const string& visLine = visLines[(size_t)contentLineIdx];
        // When wrapText is true, horizontal scroll is disabled so use relX directly.
        int col = wrapText ? relX : relX + scrollLeft;
        if (col > (int)visLine.size()) col = (int)visLine.size();
        if (col < 0) col = 0;

        // Convert visual line+col to raw text row+col
        convertVisualToRaw(contentLineIdx, col);
        dirty = true;
    }

    // Count how many visual chunks a single raw line produces when wrapped.
    int countWrappedChunks(const string& line) const {
        if (width <= 0) return 0;
        if (line.empty()) return 1;
        int count = 0, pos = 0;
        while (pos < (int)line.size()) {
            count++;
            int remaining = (int)line.size() - pos;
            if (remaining <= width) break;
            int cut = width;
            int space = line.rfind(' ', pos + width - 1);
            if (space != (int)string::npos && space > pos)
                cut = space - pos;
            pos += max(1, cut); // prevent infinite loop on long words with no spaces
            if (pos < (int)line.size() && line[pos] == ' ') pos++;
        }
        return count;
    }

    // Convert a visual (wrapped) position to raw text coordinates.
    void convertVisualToRaw(int visRow, int visCol) {
        const auto& lines = getCTextLines();
        if (!wrapText || lines.empty()) {
            cursorRow = min(visRow, max(0, (int)lines.size() - 1));
            cursorCol = min(visCol, max(0, (int)lines[cursorRow].size()));
            return;
        }

        const auto& visLines = contents;
        if (visRow >= (int)visLines.size()) {
            // Beyond last visual line — place at end of last raw line.
            cursorRow = (int)lines.size() - 1;
            cursorCol = (int)lines[cursorRow].size();
            return;
        }

        // Walk each raw line, counting its wrapped chunks to find the matching raw row+col.
        int visIdx = 0;
        for (int r = 0; r < (int)lines.size(); ++r) {
            const string& rawLine = lines[r];
            if (rawLine.empty()) {
                if (visIdx == visRow) { cursorRow = r; cursorCol = 0; return; }
                visIdx++;
                continue;
            }

            int pos = 0;
            while (pos < (int)rawLine.size()) {
                int remaining = (int)rawLine.size() - pos;
                int cut = remaining;
                if (cut > width) {
                    cut = width;
                    int space = rawLine.rfind(' ', pos + width - 1);
                    if (space != (int)string::npos && space > pos)
                        cut = space - pos;
                }

                // This chunk is visual line visIdx.
                if (visIdx == visRow) {
                    cursorRow = r;
                    cursorCol = pos + min(visCol, cut);
                    return;
                }
                visIdx++;
                pos += max(1, cut); // prevent infinite loop on long words with no spaces
                if (pos < (int)rawLine.size() && rawLine[pos] == ' ') pos++;
            }
        }

        // Fallback: place at end of last raw line.
        cursorRow = (int)lines.size() - 1;
        cursorCol = (int)lines[cursorRow].size();
    }

    // Get the visual row of the cursor for rendering.
    int getVisualCursorRow() const {
        if (!wrapText) return cursorRow;

        const auto& rawLines = getCTextLines();
        if (rawLines.empty()) return 0;

        // Walk each raw line, counting its wrapped chunks to find the visual row for cursor position.
        int visIdx = 0;
        for (int r = 0; r < (int)rawLines.size(); ++r) {
            const string& rawLine = rawLines[r];
            if (r == cursorRow) {
                // Find which chunk within this line contains cursorCol.
                int pos = 0;
                while (pos < (int)rawLine.size()) {
                    int remaining = (int)rawLine.size() - pos;
                    int cut = remaining;
                    if (cut > width) {
                        cut = width;
                        int space = rawLine.rfind(' ', pos + width - 1);
                        if (space != (int)string::npos && space > pos)
                            cut = space - pos;
                    }
                    // Use <= so cursor at chunk boundary stays on this chunk.
                    if (cursorCol <= pos + cut) return visIdx;
                    visIdx++;
                    pos += max(1, cut); // prevent infinite loop on long words with no spaces
                    if (pos < (int)rawLine.size() && rawLine[pos] == ' ') pos++;
                }
                // Cursor is at end of line — it's the last visual chunk for this row.
                return visIdx;
            }

            // Count chunks for preceding lines.
            if (rawLine.empty()) { visIdx++; continue; }
            int pos = 0;
            while (pos < (int)rawLine.size()) {
                visIdx++;
                int remaining = (int)rawLine.size() - pos;
                if (remaining <= width) break;
                int cut = width;
                int space = rawLine.rfind(' ', pos + width - 1);
                if (space != (int)string::npos && space > pos)
                    cut = space - pos;
                pos += max(1, cut); // prevent infinite loop on long words with no spaces
                if (pos < (int)rawLine.size() && rawLine[pos] == ' ') pos++;
            }
        }

        // Fallback: last visual line.
        const auto& lines = contents;
        return max(0, (int)lines.size() - 1);
    }

    // Get the visual column of the cursor within its visual row.
    int getVisualCursorCol() const {
        if (!wrapText) return cursorCol;

        const auto& rawLines = getCTextLines();
        if (rawLines.empty()) return 0;

        // Find which chunk within the current line contains cursorCol.
        const string& rawLine = rawLines[cursorRow];
        int pos = 0, lastChunkStart = 0;
        while (pos < (int)rawLine.size()) {
            int remaining = (int)rawLine.size() - pos;
            int cut = remaining;
            if (cut > width) {
                cut = width;
                int space = rawLine.rfind(' ', pos + width - 1);
                if (space != (int)string::npos && space > pos)
                    cut = space - pos;
            }
            // Use <= so cursor at chunk boundary stays on this chunk.
            if (cursorCol <= pos + cut) return max(0, min(cursorCol, pos + cut) - pos);
            lastChunkStart = pos;
            pos += max(1, cut); // prevent infinite loop on long words with no spaces
            if (pos < (int)rawLine.size() && rawLine[pos] == ' ') pos++;
        }

        // Cursor is at end of line — offset from start of last chunk.
        return max(0, cursorCol - lastChunkStart);
    }

    // Scroll so the cursor stays visible in the viewport
    void scrollToShowCursor() {
        int visualRow = getVisualCursorRow();

        if (visualRow < scrollTop) {
            setScrollTop(visualRow);
        } else if (visualRow >= scrollTop + height) {
            setScrollTop(visualRow - height + 1);
        }

        // Horizontal scroll only meaningful when wrapText is false
        if (!wrapText) {
            int visualCol = getVisualCursorCol();
            if (visualCol < scrollLeft) {
                setScrollLeft(visualCol);
            } else if (visualCol >= scrollLeft + width) {
                setScrollLeft(visualCol - width + 1);
            }
        }

        dirty = true;
    }

    // Clamp column to current line length — read-only, no mutation.
    void clampColToCurrentLine() {
        const auto& lines = getCTextLines();
        if (cursorRow >= (int)lines.size()) return;
        const string& line = lines[cursorRow];
        if (cursorCol > (int)line.size()) cursorCol = (int)line.size();
    }

    void clampCursor() {
        const auto& lines = getCTextLines();
        if (lines.empty()) {
            cursorRow = 0;
            cursorCol = 0;
            return;
        }
        if (cursorRow >= (int)lines.size()) cursorRow = (int)lines.size() - 1;
        clampColToCurrentLine();
    }

    // After modifying rawContents, update contents for display and recalculate bounds
    void updateContentsFromRaw() {
        applyWrap();
        dirty = true;
        recalculateBounds();
        clampScroll();
        scrollToShowCursor();
    }
};
