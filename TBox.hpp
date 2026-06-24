#pragma once

// DEPENDENCY: ncurses/ncurses

// Define this to make drawing slow enough to visually test any
// unnecessary/unwanted flickering and gliches:
// #define SLOWMO_TEST 10

#include <cstddef>
#include <string>
#include <vector>
#include <ncursesw/ncurses.h>

#include "../misc/array_remove.hpp"
#include "../misc/explode.hpp"
#include "../misc/implode.hpp"

#ifdef SLOWMO_TEST
#include "../misc/sleep.hpp"
#endif

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
    bool autoGrow = false;

public:

    friend class TLineup;

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
        TBox(parent, colorPair, contents ? explode("\n", contents) : vector<string>{}) {}

    TBox(TBox* parent, int width, int height, short colorPair, const char* contents = nullptr): 
        TBox(parent, width, height, colorPair, contents ? explode("\n", contents) : vector<string>{}) {}

    TBox(TBox* parent, int width, int height, int top, int left, short colorPair, const char* contents = nullptr): 
        TBox(parent, width, height, top, left, colorPair, contents ? explode("\n", contents) : vector<string>{}) {}
 
    //---

    TBox(short colorPair, const char* contents = nullptr):
        TBox(nullptr, colorPair, contents ? explode("\n", contents) : vector<string>{}) {}

    TBox(int width, int height, short colorPair, const char* contents = nullptr): 
        TBox(nullptr, width, height, colorPair, contents ? explode("\n", contents) : vector<string>{}) {}

    TBox(int width, int height, int top, int left, short colorPair, const char* contents = nullptr): 
        TBox(nullptr, width, height, top, left, colorPair, contents ? explode("\n", contents) : vector<string>{}) {}
 
        
    virtual ~TBox() {}

    // ITScrollable implementation — orientation-aware for any number of axes.
    int  getScrollValue(int o) const override {
        return (o == ITScrollable::ORIENTATION_VERTICAL) ? scrollTop : scrollLeft;
    }
    int  getScrollMin(int /*o*/) const override { return 0; }
    int  getScrollMax(int o) const override {
        if (o == ITScrollable::ORIENTATION_VERTICAL)
            return max(0, bottom - height + scrollPaddingBottom);
        return max(0, right - width + scrollPaddingRight);
    }
    void setScrollValue(int value, int o) override {
        if (o == ITScrollable::ORIENTATION_VERTICAL)
            setScrollTop(value);
        else
            setScrollLeft(value);
    }

    // -------------------------------------------------------
    // Setters
    // -------------------------------------------------------

    virtual void setSize(int width, int height) {
        if (parent && (this->width > width || this->height > height))
            parent->dirty = true;
        this->width = width;
        this->height = height;
        autoGrow = false;
        applyWrap();
        dirty = true;
        markChildrenDirty();
        // Re-clamp scroll since the visible area changed
        clampScroll();
        // Notify parent chain: our footprint may have changed (grow or shrink)
        notifyParentBoundsChange();
    }
    
    void setPosition(int top, int left) {
        this->top = top;
        this->left = left;
        if (parent) {
            parent->dirty = true;
        }
        notifyParentBoundsChange();
        dirty = true;
        markChildrenDirty();
    }

    void setContents(const vector<string>& newContents) {
        bool wasAtBottom = autoScroll && isScrollAtBottom();
        rawContents = newContents;
        applyWrap();
        dirty = true;
        recalculateBounds();
        applyAutoGrow();
        clampScroll();
        if (wasAtBottom)
            scrollToBottom();
    }

    void setContents(const string& newContents) {
        if (newContents.empty())
            setContents(vector<string>{});
        else
            setContents(explode("\n", newContents));
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

    bool isAutoScroll() const { return autoScroll; }

    void setAutoGrow(bool value) {
        autoGrow = value;
        if (autoGrow) applyAutoGrow();
    }

    bool isAutoGrow() const { return autoGrow; }

    void scrollToBottom() {
        setScrollTop(max(0, bottom - height + scrollPaddingBottom));
    }

    void setWrapText(bool value) {
        wrapText = value;
        applyWrap();
        dirty = true;
        recalculateBounds();
        clampScroll();
        if (parent) parent->recalc();
    }

    // Placeholder for any child class to recalculate after 
    virtual void recalc() {}

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

    bool isWrapText() const { return wrapText; }

    // Returns the wrapped content lines (after applyWrap).
    vector<string> getWrappedContents() const { return contents; }

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
        TBox* hit = findMouseAt(x, y);
        while (hit != nullptr) {
            if (hit->scrollable) return hit;
            // Walk up: only consider ancestors that also contain the point.
            if (!hit->contains(x, y)) break;
            hit = hit->parent;
        }
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

    virtual void addChild(TBox* child) {
        for (TBox* child_: children)
            if (child_ == child) return;

        bool wasAtBottom = autoScroll && isScrollAtBottom();
        if (child->parent) child->parent->dirty = true;
        child->dirty = true;
        dirty = true;
        children.push_back(child);
        child->parent = this;
        recalculateBounds();
        applyAutoGrow();
        if (wasAtBottom)
            scrollToBottom();
    }

    // -------------------------------------------------------
    // Event handlers
    // -------------------------------------------------------

    virtual TEventResult onKeyPress(int key, string name) override {
        TEventResult result = TEventHandler::onKeyPress(key, name);
        if (result & TEventResult::Stop) return result;

        // NOTE: implement universal event handlings here if necessary

        return result;
    }

    virtual TEventResult onMouseMove(int x, int y) override {
        TEventResult result = TEventHandler::onMouseMove(x, y);
        if (result & TEventResult::Stop) return result;

        // NOTE: implement universal event handlings here if necessary

        return result;
    }

    virtual TEventResult onMouseClick(int x, int y, unsigned int button, unsigned int repeat) override {
        TEventResult result = TEventHandler::onMouseClick(x, y, button, repeat);
        if (result & TEventResult::Stop) return result;

        // NOTE: implement universal event handlings here if necessary

        return result;
    }

    virtual TEventResult onMouseDown(int x, int y, unsigned int button) override {
        TEventResult result = TEventHandler::onMouseDown(x, y, button);
        if (result & TEventResult::Stop) return result;

        // NOTE: implement universal event handlings here if necessary

        return result;
    }

    virtual TEventResult onMouseUp(int x, int y, unsigned int button) override {
        TEventResult result = TEventHandler::onMouseUp(x, y, button);
        if (result & TEventResult::Stop) return result;

        // NOTE: implement universal event handlings here if necessary

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

        for (TBox* child : children) {
            TEventResult childResult = child->onResize(cols, rows);
            result |= childResult;
            if (childResult & TEventResult::Stop) return result;
        }

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

        // Horizontal extent: when wrapping is enabled the visual lines all fit
        // within 'width', so we use wrapped contents to report zero scrollable
        // width.  When wrapping is disabled we use rawContents so that long
        // lines produce a proper horizontal scroll range for the scrollbar.
        const vector<string>& hLines = wrapText ? contents : rawContents;
        int contentRight  = 0;
        for (const string& line : hLines)
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

    // When autoGrow is enabled, adjusts height/width to match content extent.
    void applyAutoGrow() {
        if (!autoGrow) return;
        int newHeight = bottom;
        int newWidth  = wrapText ? width : right;
        if (newHeight != height || newWidth != width) {
            height = newHeight;
            width  = newWidth;
            dirty = true;
            markChildrenDirty();
            clampScroll();
        }
        notifyParentBoundsChange();
    }

    // Propagates a bounds change up the ancestor chain.
    virtual void notifyParentBoundsChange() {
        TBox* node = parent;
        while (node) {
            bool wasAtBottom = node->autoScroll && node->isScrollAtBottom();

            int oldBottom = node->bottom, oldRight  = node->right;
            int oldHeight = node->height, oldWidth   = node->width;

            // Recalculate from updated children/contents (reuses existing method)
            node->recalculateBounds();

            // Apply auto-grow on this ancestor if enabled
            if (node->autoGrow) {
                int gHeight = node->bottom;
                int gWidth  = node->wrapText ? node->width : node->right;
                if (gHeight != node->height || gWidth != node->width) {
                    node->height = gHeight;
                    node->width  = gWidth;
                }
            }

            // Clamp scroll and auto-scroll if anything changed
            bool changed = (node->bottom != oldBottom || node->right != oldRight
                         || node->height != oldHeight || node->width != oldWidth);
            if (changed) {
                int maxTop  = max(0, node->bottom - node->height + node->scrollPaddingBottom);
                int maxLeft = max(0, node->right  - node->width  + node->scrollPaddingRight);
                node->scrollTop  = max(0, min(node->scrollTop,  maxTop));
                node->scrollLeft = max(0, min(node->scrollLeft, maxLeft));

                if (wasAtBottom) {
                    node->setScrollTop(max(0, node->bottom - node->height + node->scrollPaddingBottom));
                }
            }

            // Always mark dirty so the ancestor redraws with updated child layout.
            node->dirty = true;
            node->markChildrenDirty();

            node = node->parent;
        }
    }

    // Clamps current scroll values to the valid range.
    // Call after any change to bottom, right, width, or height.
    void clampScroll() {
        int maxScrollTop  = max(0, bottom - height + scrollPaddingBottom);
        int maxScrollLeft = max(0, right  - width  + scrollPaddingRight);
        scrollTop  = max(0, min(scrollTop,  maxScrollTop));
        scrollLeft = max(0, min(scrollLeft, maxScrollLeft));
    }

    virtual void redraw() {
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

                // Try to find the last space within the current line width.
                int cut = width;
                int spaceWithin = line.rfind(' ', pos + width - 1);
                if (spaceWithin != (int)string::npos && spaceWithin > pos) {
                    cut = spaceWithin - pos;
                } else {
                    // No space within width — look forward for the next space
                    // so we can break at a word boundary instead of mid-word.
                    int nextSpace = line.find(' ', pos + 1);
                    if (nextSpace != (int)string::npos && nextSpace > pos) {
                        cut = nextSpace - pos;
                    }
                }

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
#ifdef SLOWMO_TEST
        refresh();
        sleep_ms(SLOWMO_TEST);
#endif
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
        applyAutoGrow(); // shrink if auto-grow enabled
        notifyParentBoundsChange(); // cascade up the chain
    }
};
