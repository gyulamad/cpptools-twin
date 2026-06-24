#pragma once

#include "TBox.hpp"
#include "TOrientation.hpp"

// TLineup auto-grows and automatically places the next added box
// aligned to the others in-line.
// The TLineup can be defined as HORIZONTAL or VERTICAL (similarly
// to TScrollbar).

class TLineup: public TBox {
private:
    TOrientation orientation = HORIZONTAL;
    int gap = 0;
    int paddingTop = 0;
    int paddingLeft = 0;
    int paddingRight = 0;
    int paddingBottom = 0;
    bool fitChildren = false;

public:
    TLineup(TBox* parent, short colorPair, TOrientation o = HORIZONTAL):
        TBox(parent, colorPair, vector<string>{}), orientation(o)
    {
        autoGrow = true;
    }

    TLineup(TBox* parent, int width, int height, int top, int left, short colorPair, TOrientation o = HORIZONTAL):
        TBox(parent, width, height, top, left, colorPair), orientation(o)
    {
        autoGrow = true;
    }

    // --- No-parent overloads (parent is nullptr; use setParent() later) ---

    TLineup(short colorPair, TOrientation o = HORIZONTAL):
        TLineup(nullptr, colorPair, o) {}

    TLineup(int width, int height, short colorPair, TOrientation o = HORIZONTAL):
        TLineup(nullptr, width, height, 0, 0, colorPair, o) {}

    TLineup(int width, int height, int top, int left, short colorPair, TOrientation o = HORIZONTAL):
        TLineup(nullptr, width, height, top, left, colorPair, o) {}

    virtual ~TLineup() {}

    void setGap(int newGap) {
        gap = newGap;
        relayout();
    }

    int getGap() const { return gap; }

    TOrientation getOrientation() const { return orientation; }

    // --- Padding getters/setters (trigger relayout on change) ---

    void setPaddingTop(int v) { paddingTop = v; relayout(); }
    int getPaddingTop() const { return paddingTop; }

    void setPaddingLeft(int v) { paddingLeft = v; relayout(); }
    int getPaddingLeft() const { return paddingLeft; }

    void setPaddingRight(int v) { paddingRight = v; relayout(); }
    int getPaddingRight() const { return paddingRight; }

    void setPaddingBottom(int v) { paddingBottom = v; relayout(); }
    int getPaddingBottom() const { return paddingBottom; }

    // Set all paddings at once (avoids multiple relayouts).
    void setPaddings(int top, int left, int right, int bottom) {
        paddingTop = top;
        paddingLeft = left;
        paddingRight = right;
        paddingBottom = bottom;
        relayout();
    }

    // --- fitChildren: stretch children perpendicular to layout direction ---

    void setFitChildren(bool v) {
        fitChildren = v;
        relayout();
    }

    bool getFitChildren() const { return fitChildren; }

    void addChild(TBox* child) override {
        if (children.empty()) {
            // First child starts at padding offset.
            positionChildAt(child, 0);
        } else {
            positionChild(child);
        }

        // Call base addChild to register the child and update bounds.
        TBox::addChild(child);

        // Re-extend bottom/right so trailing padding survives the auto-grow in base addChild.
        recalculateBounds();
        bottom += paddingBottom;
        right += paddingRight;
        applyAutoGrow();
    }

   void relayout() {
        for (int i = 0; i < (int)children.size(); ++i) {
            positionChildAt(children[i], i);
        }
        dirty = true;
        recalculateBounds();
        // Extend bounds so trailing padding is included in auto-grow.
        bottom += paddingBottom;
        right += paddingRight;
        if (!children.empty()) applyAutoGrow();
    }

private:
    void positionChild(TBox* child) {
        int idx = (int)children.size();
        positionChildAt(child, idx);
    }

    void positionChildAt(TBox* child, int idx) {
        if (orientation == HORIZONTAL) {
            // Cumulative left: padding + sum of previous widths and gaps.
            int cumulativeLeft = paddingLeft;
            for (int i = 0; i < idx; ++i) {
                cumulativeLeft += children[i]->getWidth() + gap;
            }

            // Align top to min sibling top, but never above paddingTop.
            int minTop = paddingTop;
            if (!children.empty()) {
                minTop = children[0]->getTop();
                for (TBox* c : children)
                    minTop = min(minTop, c->getTop());
                minTop = max(minTop, paddingTop);
            }

            child->setPosition(minTop, cumulativeLeft);

            // fitChildren: stretch child height to inner space.
            if (fitChildren && height > 0) {
                int innerHeight = height - paddingTop - paddingBottom;
                if (innerHeight < 1) innerHeight = 1;
                child->setSize(child->getWidth(), innerHeight);
            }
        } else {
            // Cumulative top: padding + sum of previous heights and gaps.
            int cumulativeTop = paddingTop;
            for (int i = 0; i < idx; ++i) {
                cumulativeTop += children[i]->getHeight() + gap;
            }

            // Align left to min sibling left, but never before paddingLeft.
            int minLeft = paddingLeft;
            if (!children.empty()) {
                minLeft = children[0]->getLeft();
                for (TBox* c : children)
                    minLeft = min(minLeft, c->getLeft());
                minLeft = max(minLeft, paddingLeft);
            }

            child->setPosition(cumulativeTop, minLeft);

            // fitChildren: stretch child width to inner space.
            if (fitChildren && width > 0) {
                int innerWidth = width - paddingLeft - paddingRight;
                if (innerWidth < 1) innerWidth = 1;
                child->setSize(innerWidth, child->getHeight());
            }
        }
    }
};
