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

public:
    void setGap(int newGap) {
        gap = newGap;
        recalc();
    }

    int getGap() const { return gap; }

    TOrientation getOrientation() const { return orientation; }

    // --- Padding getters/setters (trigger relayout on change) ---

    void setPaddingTop(int v) { paddingTop = v; recalc(); }
    int getPaddingTop() const { return paddingTop; }

    void setPaddingLeft(int v) { paddingLeft = v; recalc(); }
    int getPaddingLeft() const { return paddingLeft; }

    void setPaddingRight(int v) { paddingRight = v; recalc(); }
    int getPaddingRight() const { return paddingRight; }

    void setPaddingBottom(int v) { paddingBottom = v; recalc(); }
    int getPaddingBottom() const { return paddingBottom; }

    // Set all paddings at once (avoids multiple relayouts).
    void setPaddings(int top, int left, int right, int bottom) {
        paddingTop = top;
        paddingLeft = left;
        paddingRight = right;
        paddingBottom = bottom;
        recalc();
    }

    // --- fitChildren: stretch children perpendicular to layout direction ---

    void setFitChildren(bool v) {
        fitChildren = v;
        recalc();
    }

    bool getFitChildren() const { return fitChildren; }

    void addChild(TBox* child) override {
        if (children.empty()) {
            positionChildAt(child, 0);
        } else {
            positionChild(child);
        }

        // Disable autoGrow so TBox::addChild does not shrink our dimensions.
        // recalc() will applyAutoGrow at the end with paddings accounted for.
        bool saved = autoGrow;
        autoGrow = false;
        TBox::addChild(child);
        autoGrow = saved;

        recalc();
    }

    // Recalculates all children positions and sizes.
    // Restarts from the first child whenever one changes, so siblings
    // see updated dimensions immediately. Stops when a full pass produces
    // no changes (convergence).
    void recalc() {
        if (children.empty()) return;

        for (int i = 0; i < (int)children.size(); ++i) {
            int oldW = children[i]->getWidth();
            int oldH = children[i]->getHeight();
            int oldT = children[i]->getTop();
            int oldL = children[i]->getLeft();

            // Disable autoGrow during positioning so child->setSize
            // does not trigger notifyParentBoundsChange which would
            // shrink our explicit dimensions before we finalize them.
            bool savedAutoGrow = autoGrow;
            autoGrow = false;
            positionChildAt(children[i], i);
            autoGrow = savedAutoGrow;

            if (children[i]->getWidth() != oldW || children[i]->getHeight() != oldH ||
                children[i]->getTop() != oldT || children[i]->getLeft() != oldL) {
                i = -1;
                continue; // restart from first child with updated dimensions
            }
        }

        dirty = true;
        recalculateBounds();
        // Extend bounds so trailing padding is included in auto-grow.
        bottom += paddingBottom;
        right += paddingRight;
        applyAutoGrow();
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

            // Store current flow dimension before setSize may trigger wrapping.
            int prevFlow = horiz ? child->getWidth() : child->getHeight();

            child->setSize(horiz ? child->getWidth() : innerPerp, horiz ? innerPerp : child->getHeight());

            // After setSize triggers applyWrap, the content extent (bottom) may have grown.
            // Recalculate so getBottom reflects wrapped line count, then grow flow dimension if needed.
            child->recalculateBounds();
            int newFlow = max(child->getBottom(), prevFlow);
            if (horiz && newFlow != child->getWidth()) {
                child->setSize(newFlow, child->getHeight());
            } else if (!horiz && newFlow != child->getHeight()) {
                child->setSize(child->getWidth(), newFlow);
            }
        }
    }
};
