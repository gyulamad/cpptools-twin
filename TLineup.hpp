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
        bool horiz = (orientation == HORIZONTAL);

        // Flow axis: cumulative offset along layout direction.
        int flowPadding = horiz ? paddingLeft : paddingTop;
        int cumFlow = flowPadding;
        for (int i = 0; i < idx; ++i) {
            cumFlow += (horiz ? children[i]->getWidth() : children[i]->getHeight()) + gap;
        }

        // Perpendicular axis: align to min sibling, clamped by padding.
        int perpPadding = horiz ? paddingTop : paddingLeft;
        int perpAlign = perpPadding;
        if (!children.empty()) {
            perpAlign = (horiz ? children[0]->getTop() : children[0]->getLeft());
            for (TBox* c : children)
                perpAlign = min(perpAlign, horiz ? c->getTop() : c->getLeft());
            perpAlign = max(perpAlign, perpPadding);
        }

        if (horiz)
            child->setPosition(perpAlign, cumFlow);
        else
            child->setPosition(cumFlow, perpAlign);

        // fitChildren: stretch perpendicular dimension to inner space.
        if (fitChildren && height > 0 && width > 0) {
            int innerPerp = horiz ? (height - paddingTop - paddingBottom) : (width - paddingLeft - paddingRight);
            if (innerPerp < 1) innerPerp = 1;
            child->setSize(horiz ? child->getWidth() : innerPerp, horiz ? innerPerp : child->getHeight());
        }
    }
};
