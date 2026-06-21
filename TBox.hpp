#pragma once

// DEPENDENCY: ncurses/ncurses

#include <cstddef>
#include <string>
#include <vector>
#include <ncursesw/ncurses.h>

#include "../misc/array_remove.hpp"
#include "../misc/explode.hpp"
#include "../misc/implode.hpp"

#include "ITScrollable.hpp"
#include "TEventHandler.hpp"

using namespace std;


class TBox: public TEventHandler, public ITScrollable {
protected:
    bool dirty = true;
    TBox* parent = nullptr;
    int width, height, top, left;
    short colorPair;
    vector<string> contents;
    bool wrapText = false;
    vector<string> rawContents;
    vector<TBox*> children;
    int bottom = 0, right = 0;
    int scrollTop = 0, scrollLeft = 0;
    int scrollPaddingBottom = 0, scrollPaddingRight = 0;
    bool scrollable = false;
    int scrollSpeed = 3; // lines per scroll tick
    bool autoScroll = false;

public:

    TBox(TBox* parent, short colorPair, vector<string> contents):
        parent(parent), width(0), height(contents.size()), top(0), left(0), colorPair(colorPair)
    {
        for (const string& content: contents)
            width = max((size_t)width, content.length());
        if(parent) parent->addChild(this);
        recalculateBounds();
        setContents(contents);
    }
   
    TBox(TBox* parent, int width, int height, short colorPair, vector<string> contents): 
        parent(parent), width(width), height(height), top(0), left(0), colorPair(colorPair)
    {
        if(parent) parent->addChild(this);
        recalculateBounds();
        setContents(contents);
    }
    
    TBox(TBox* parent, int width, int height, int top, int left, short colorPair, vector<string> contents): 
        parent(parent), width(width), height(height), top(top), left(left), colorPair(colorPair)
    {
        if(parent) parent->addChild(this);
        recalculateBounds();
        setContents(contents);
    }
    
    //---
    
    TBox(short colorPair, vector<string> contents):
        TBox(nullptr, colorPair, contents) {}

    TBox(int width, int height, short colorPair, vector<string> contents): 
        TBox(nullptr, width, height, colorPair, contents) {}

    TBox(int width, int height, int top, int left, short colorPair, vector<string> contents): 
        TBox(nullptr, width, height, top, left, colorPair, contents) {}
        
    //---
    
    TBox(TBox* parent, short colorPair, const string& contents):
        TBox(parent, colorPair, explode("\n", contents)) {}

    TBox(TBox* parent, int width, int height, short colorPair, const string& contents): 
        TBox(parent, width, height, colorPair, explode("\n", contents)) {}

    TBox(TBox* parent, int width, int height, int top, int left, short colorPair, const string& contents): 
        TBox(parent, width, height, top, left, colorPair, explode("\n", contents)) {}
    
    //---
    
    TBox(short colorPair, const string& contents):
        TBox(nullptr, colorPair, explode("\n", contents)) {}

    TBox(int width, int height, short colorPair, const string& contents): 
        TBox(nullptr, width, height, colorPair, explode("\n", contents)) {}

    TBox(int width, int height, int top, int left, short colorPair, const string& contents): 
        TBox(nullptr, width, height, top, left, colorPair, explode("\n", contents)) {}
 
    //---

    TBox(TBox* parent, short colorPair, const char* contents = nullptr):
        TBox(parent, colorPair, explode("\n", contents ? contents : "")) {}

    TBox(TBox* parent, int width, int height, short colorPair, const char* contents = nullptr): 
        TBox(parent, width, height, colorPair, explode("\n", contents ? contents : "")) {}

    TBox(TBox* parent, int width, int height, int top, int left, short colorPair, const char* contents = nullptr): 
        TBox(parent, width, height, top, left, colorPair, explode("\n", contents ? contents : "")) {}
 
    //---

    TBox(short colorPair, const char* contents = nullptr):
        TBox(nullptr, colorPair, explode("\n", contents ? contents : "")) {}

    TBox(int width, int height, short colorPair, const char* contents = nullptr): 
        TBox(nullptr, width, height, colorPair, explode("\n", contents ? contents : "")) {}

    TBox(int width, int height, int top, int left, short colorPair, const char* contents = nullptr): 
        TBox(nullptr, width, height, top, left, colorPair, explode("\n", contents ? contents : "")) {}
 
        
    virtual ~TBox() {}

    // ITScrollable implementation (vertical)
    int  getScrollValue() const override { return scrollTop; }
    int  getScrollMin()   const override { return 0; }
    int  getScrollMax()   const override { return max(0, bottom - height + scrollPaddingBottom); }
    void setScrollValue(int value) override { setScrollTop(value); }

    // -------------------------------------------------------
    // Setters
    // -------------------------------------------------------

    virtual void setSize(int width, int height) {
        if (parent && (this->width > width || this->height > height))
            parent->dirty = true;
        this->width = width;
        this->height = height;
        applyWrap();
        dirty = true;
        markChildrenDirty();
        // Re-clamp scroll since the visible area changed
        clampScroll();
        // Notify parent: our footprint may have changed
        if (parent) parent->recalculateBounds();
    }
    
    void setPosition(int top, int left) {
        this->top = top;
        this->left = left;
        if (parent) {
            parent->dirty = true;
            parent->recalculateBounds();
        }
        dirty = true;
        markChildrenDirty();
    }

    void setContents(const vector<string>& newContents) {
        rawContents = newContents;
        applyWrap();
        dirty = true;
        recalculateBounds();
        clampScroll();
    }

    void setContents(const string& newContents) {
        bool atBottom = isScrollAtBottom();
        setContents(explode("\n", newContents));
        if (autoScroll && atBottom)
            scrollToBottom();
    }

    void setColorPair(short colorPair) {
        this->colorPair = colorPair;
    }

    void setParent(TBox* parent) {
        if (this->parent) this->parent->removeChild(this);
        if (parent) parent->addChild(this);
    }

    void setScrollTop(int newScrollTop) {
        int maxScroll = max(0, bottom - height + scrollPaddingBottom);
        scrollTop = max(0, min(newScrollTop, maxScroll));
        dirty = true;
        markChildrenDirty();
        notifyScrollChange();
    }

    void setScrollLeft(int newScrollLeft) {
        int maxScroll = max(0, right - width + scrollPaddingRight);
        scrollLeft = max(0, min(newScrollLeft, maxScroll));
        dirty = true;
        markChildrenDirty();
        notifyScrollChange();
    }

    void setScrollPaddingBottom(int padding) {
        scrollPaddingBottom = padding;
        clampScroll();
    }

    void setScrollPaddingRight(int padding) {
        scrollPaddingRight = padding;
        clampScroll();
    }

    // opt a box into receiving and handling scroll events
    void setScrollable(bool value) {
        scrollable = value;
    }

    // set how many lines are scrolled per wheel tick (default: 3)
    void setScrollSpeed(int speed) {
        scrollSpeed = max(1, speed);
    }

    void setAutoScroll(bool value) {
        autoScroll = value;
    }
    
    void scrollToBottom() {
        setScrollTop(max(0, bottom - height + scrollPaddingBottom));
    }

    void setWrapText(bool value) {
        wrapText = value;
        applyWrap();
        dirty = true;
        recalculateBounds();
        clampScroll();
    }

    // -------------------------------------------------------
    // Getters
    // -------------------------------------------------------

    int getWidth() const { return width;  }
    int getHeight() const { return height; }
    int getTop() const { return top;  }
    int getLeft() const { return left; }
    int getScrollTop() const { return scrollTop; }
    int getScrollLeft() const { return scrollLeft; }
    bool isScrollable() const { return scrollable; }

    int getTopAbsolute() const {
        return top + (parent ? parent->getTopAbsolute() - parent->scrollTop : 0);
    }

    int getLeftAbsolute() const {
        return left + (parent ? parent->getLeftAbsolute() - parent->scrollLeft : 0);
    }

    int getBottom() const { return bottom; }
    int getRight()  const { return right;  }

    string getContents() const {
        return implode("\n", rawContents);
    }

    TBox* getParent() const { return parent; }

    bool isScrollAtBottom() const {
        int maxScroll = max(0, bottom - height + scrollPaddingBottom);
        return scrollTop >= maxScroll;
    }

    // -------------------------------------------------------
    // Hit-testing
    // -------------------------------------------------------

    // Returns true when the absolute screen position (x, y) falls inside
    // this box's visible (unclipped) area.
    bool contains(int x, int y) const {
        int absTop  = getTopAbsolute();
        int absLeft = getLeftAbsolute();
        return x >= absLeft && x < absLeft + width
            && y >= absTop  && y < absTop  + height;
    }

    // Returns the deepest elemen descendant (or this box itself) 
    // that contains (x, y).
    // Children are tested last-to-first so that the topmost drawn child
    // wins when boxes overlap.
    TBox* findMouseAt(int x, int y) {
        if (!contains(x, y)) return nullptr;

        // Depth-first: prefer deepest child
        for (int i = (int)children.size() - 1; i >= 0; --i) {
            TBox* hit = children[i]->findMouseAt(x, y);
            if (hit) return hit;
        }

        return this;
    }

    // Returns the deepest scrollable descendant (or this box itself if
    // scrollable) that contains (x, y), or nullptr if none does.
    TBox* findScrollableAt(int x, int y) {
        // No child claimed it — return this box if it can scroll
        TBox* that = findMouseAt(x, y);
        if (that && that->scrollable) return that;
        return nullptr;
    }

    // -------------------------------------------------------
    // Tree management
    // -------------------------------------------------------

    void draw() {
        if (dirty) redraw();
        for (TBox* child: children) child->draw();
        dirty = false;
    }

    void addChild(TBox* child) {
        for (TBox* child_: children)
            if (child_ == child) return;

        if (child->parent) child->parent->dirty = true;
        child->dirty = true;
        dirty = true;
        children.push_back(child);
        child->parent = this;
        recalculateBounds();
    }

    // -------------------------------------------------------
    // Event handlers
    // -------------------------------------------------------

    virtual TEventResult onKeyPress(int key, string name) override {
        TEventResult result = TEventHandler::onKeyPress(key, name);
        if (result & TEventResult::Stop) return result;

        // TODO implement universal event handlings here if necessary

        return result;
    }

    virtual TEventResult onMouseMove(int x, int y) override {
        TEventResult result = TEventHandler::onMouseMove(x, y);
        if (result & TEventResult::Stop) return result;

        // TODO implement universal event handlings here if necessary

        return result;
    }

    virtual TEventResult onMouseClick(int x, int y, unsigned int button, unsigned int repeat) override {
        TEventResult result = TEventHandler::onMouseClick(x, y, button, repeat);
        if (result & TEventResult::Stop) return result;

        // TODO implement universal event handlings here if necessary

        return result;
    }

    virtual TEventResult onMouseDown(int x, int y, unsigned int button) override {
        TEventResult result = TEventHandler::onMouseDown(x, y, button);
        if (result & TEventResult::Stop) return result;

        // TODO implement universal event handlings here if necessary

        return result;
    }

    virtual TEventResult onMouseUp(int x, int y, unsigned int button) override {
        TEventResult result = TEventHandler::onMouseUp(x, y, button);
        if (result & TEventResult::Stop) return result;

        // TODO implement universal event handlings here if necessary

        return result;
    }

    virtual TEventResult onMouseScroll(int x, int y, unsigned int direction) override {
        TEventResult result = TEventHandler::onMouseScroll(x, y, direction);
        if (result & TEventResult::Stop) return result;
        if (!scrollable) return result;
        // direction 4 = wheel up (scroll content up → decrement scrollTop)
        // direction 5 = wheel down (scroll content down → increment scrollTop)
        int delta = (direction == 4) ? -scrollSpeed : scrollSpeed;
        setScrollTop(scrollTop + delta);
        return result;
    }

    virtual TEventResult onResize(int cols, int rows) override {
        TEventResult result = TEventHandler::onResize(cols, rows);
        if (result & TEventResult::Stop) return result;

        // TODO implement universal event handlings here if necessary

        return result;
    }

protected:

    // Recalculates bottom/right from contents and children.
    // Call whenever contents or children change.
    // Does NOT propagate upward — callers that affect the parent's
    // bounds (addChild, removeChild, setPosition, setSize) do so explicitly.
    void recalculateBounds() {
        // Extent from contents
        int contentBottom = (int)contents.size();
        int contentRight  = 0;
        for (const string& line : contents)
            contentRight = max(contentRight, (int)line.size());

        // Extent from children
        int childBottom = 0;
        int childRight  = 0;
        for (const TBox* child : children) {
            childBottom = max(childBottom, child->top  + child->height);
            childRight  = max(childRight,  child->left + child->width);
        }

        bottom = max(contentBottom, childBottom);
        right  = max(contentRight,  childRight);
    }

    // Clamps current scroll values to the valid range.
    // Call after any change to bottom, right, width, or height.
    void clampScroll() {
        int maxScrollTop  = max(0, bottom - height + scrollPaddingBottom);
        int maxScrollLeft = max(0, right  - width  + scrollPaddingRight);
        scrollTop  = max(0, min(scrollTop,  maxScrollTop));
        scrollLeft = max(0, min(scrollLeft, maxScrollLeft));
    }

    virtual void redraw() { // TODO: = 0;
        attron(COLOR_PAIR(colorPair));

        // Absolute bounds of this box
        int absTop  = getTopAbsolute();
        int absLeft = getLeftAbsolute();

        // Clip region: constrain to parent's absolute bounds (if we have a parent)
        int clipTop    = absTop;
        int clipLeft   = absLeft;
        int clipBottom = absTop  + height;
        int clipRight  = absLeft + width;

        TBox* ancestor = parent;
        while (ancestor) {
            int aTop    = ancestor->getTopAbsolute();
            int aLeft   = ancestor->getLeftAbsolute();
            clipTop     = max(clipTop,    aTop);
            clipLeft    = max(clipLeft,   aLeft);
            clipBottom  = min(clipBottom, aTop  + ancestor->height);
            clipRight   = min(clipRight,  aLeft + ancestor->width);
            ancestor    = ancestor->parent;
        }

        // Nothing visible at all
        if (clipTop >= clipBottom || clipLeft >= clipRight) {
            attroff(COLOR_PAIR(colorPair));
            return;
        }

        for (int line = 0; line < height; line++) {
            int screenRow = absTop + line;

            // Skip rows outside the clipped vertical range
            if (screenRow < clipTop || screenRow >= clipBottom) continue;

            // Determine the horizontal slice to draw
            int drawLeft  = clipLeft;
            int drawRight = clipRight;

            // Offset into the content string, accounting for left clip and scroll
            int contentOffset = (drawLeft - absLeft) + scrollLeft;
            int drawWidth     = drawRight - drawLeft;

            // Get content for this line, accounting for vertical scroll
            int contentLine = line + scrollTop;
            string content = (contentLine < (int)contents.size()) ? contents[contentLine] : "";

            // Slice [contentOffset, contentOffset + drawWidth), pad with ' '
            string str;
            if (contentOffset < (int)content.size())
                str = content.substr(contentOffset, drawWidth);

            if ((int)str.size() < drawWidth)
                str += string(drawWidth - str.size(), ' ');

            putstrxy(screenRow, drawLeft, str.c_str());
        }

        attroff(COLOR_PAIR(colorPair));
    }

    // Prints `str` at a position RELATIVE to this box's top-left corner,
    // accounting for scroll offset. Clips against this box and all ancestors.
    // Call attron(COLOR_PAIR(...)) before using this.
    void print(int row, int col, const string& str) {
        // Derive absolute screen position from relative coords at draw time
        int screenRow = getTopAbsolute()  + row;
        int screenCol = getLeftAbsolute() + col;

        // Build clip region from this box and all ancestors
        int clipTop    = getTopAbsolute();
        int clipLeft   = getLeftAbsolute();
        int clipBottom = clipTop  + height;
        int clipRight  = clipLeft + width;

        TBox* ancestor = parent;
        while (ancestor) {
            int aTop   = ancestor->getTopAbsolute();
            int aLeft  = ancestor->getLeftAbsolute();
            clipTop    = max(clipTop,    aTop);
            clipLeft   = max(clipLeft,   aLeft);
            clipBottom = min(clipBottom, aTop  + ancestor->height);
            clipRight  = min(clipRight,  aLeft + ancestor->width);
            ancestor   = ancestor->parent;
        }

        if (clipTop >= clipBottom || clipLeft >= clipRight) return;
        if (screenRow < clipTop    || screenRow >= clipBottom) return;
        if (screenCol < clipLeft   || screenCol >= clipRight)  return;

        putstrxy(screenRow, screenCol, str.c_str());
    }

    void applyWrap() {
        if (!wrapText || width <= 0) {
            contents = rawContents;
            return;
        }
        contents.clear();
        for (const string& line : rawContents) {
            if (line.empty()) { contents.push_back(""); continue; }
            int pos = 0;
            while (pos < (int)line.size()) {
                int remaining = (int)line.size() - pos;
                if (remaining <= width) {
                    contents.push_back(line.substr(pos));
                    break;
                }
                // Find last space within width
                int cut = width;
                int space = line.rfind(' ', pos + width - 1);
                if (space != (int)string::npos && space > pos)
                    cut = space - pos;
                contents.push_back(line.substr(pos, cut));
                pos += cut;
                // Skip the space we broke on
                if (pos < (int)line.size() && line[pos] == ' ') pos++;
            }
        }
    }

private:

    // Single point of responsibility for cursor-neutral text output.
    // "Whatever moves the cursor shall restore it."
    // Callers must ensure attron(COLOR_PAIR(...)) is active before calling.
    static void putstrxy(int row, int col, const char* str) {
        int savedRow, savedCol;
        getyx(stdscr, savedRow, savedCol);
        move(row, col);
        printw("%s", str);
        move(savedRow, savedCol);
    }

    void markChildrenDirty() {
        for (TBox* child : children) {
            child->dirty = true;
            child->markChildrenDirty();
        }
    }

    void removeChild(TBox* child) {
        children = array_remove(children, child);
        child->parent = nullptr;
        dirty = true;
        recalculateBounds();
        clampScroll();
    }
};
