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

public:
    TLineup(TBox* parent, short colorPair, TOrientation o = HORIZONTAL):
        TBox(parent, colorPair, vector<string>{}), orientation(o)
    {
        setAutoGrow(true);
    }

    TLineup(TBox* parent, int width, int height, int top, int left, short colorPair, TOrientation o = HORIZONTAL):
        TBox(parent, width, height, top, left, colorPair), orientation(o)
    {
        setAutoGrow(true);
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

    void addChild(TBox* child) override {
        if (children.empty()) {
            // First child starts at origin
            child->setPosition(0, 0);
        } else {
            positionChild(child);
        }

        // Call base addChild to register the child and update bounds
        TBox::addChild(child);
    }

    void relayout() {
        for (int i = 0; i < (int)children.size(); ++i) {
            positionChildAt(children[i], i);
        }
        dirty = true;
        recalculateBounds();
        applyAutoGrow();
    }

private:
    void positionChild(TBox* child) {
        int idx = (int)children.size();
        positionChildAt(child, idx);
    }

    void positionChildAt(TBox* child, int idx) {
        if (orientation == HORIZONTAL) {
            int cumulativeLeft = 0;
            for (int i = 0; i < idx; ++i) {
                cumulativeLeft += children[i]->getWidth() + gap;
            }
            // Align to the minimum top among siblings
            int minTop = 0;
            if (!children.empty()) {
                minTop = children[0]->getTop();
                for (TBox* c : children)
                    minTop = min(minTop, c->getTop());
            }
            child->setPosition(minTop, cumulativeLeft);
        } else {
            int cumulativeTop = 0;
            for (int i = 0; i < idx; ++i) {
                cumulativeTop += children[i]->getHeight() + gap;
            }
            // Align to the minimum left among siblings
            int minLeft = 0;
            if (!children.empty()) {
                minLeft = children[0]->getLeft();
                for (TBox* c : children)
                    minLeft = min(minLeft, c->getLeft());
            }
            child->setPosition(cumulativeTop, minLeft);
        }
    }
};
