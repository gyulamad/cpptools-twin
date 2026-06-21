#pragma once

#include "TBox.hpp"
#include "TEventResult.hpp"

class TScrollbar: public TBox {
public:
    enum Orientation { VERTICAL, HORIZONTAL };

    struct Style {
        const char* trackV;
        const char* thumbV;
        const char* arrowUp;
        const char* arrowDn;
        const char* trackH;
        const char* thumbH;
        const char* arrowLt;
        const char* arrowRt;
    };

    static constexpr Style ASCII    = { "|", "#", "^", "v", "-", "#", "<", ">" };
    static constexpr Style UNICODE  = { "|", "█", "▲", "▼", "─", "█", "◄", "►" };

private:
    ITScrollable* target      = nullptr;
    Orientation  orientation = VERTICAL;
    Style        style;
    bool         dragging    = false;
    int          dragAnchor  = 0;
    int          dragOrigin  = 0;
    int          thumbPos    = 0;
    int          thumbSize   = 1;

public:
    // Auto-position: derives top/left/length from the target TBox
    TScrollbar(TBox* target, short colorPair, Orientation orientation = VERTICAL, Style style = ASCII): 
        TBox(nullptr, 0, 0, 0, 0, colorPair), orientation(orientation), style(style)
    {
        setParent(target->getParent());
        setScrollable(true);
        setTarget(target);
    }

    // Auto-position: derives top/left/length from the target TBox
    TScrollbar(TBox* attachParent, TBox* target, short colorPair, Orientation orientation = VERTICAL, Style style = ASCII): 
        TBox(attachParent, 0, 0, 0, 0, colorPair), orientation(orientation), style(style)
    {
        setScrollable(true);
        setTarget(target);
    }

    // Manual-position: for non-TBox ITScrollable targets (e.g. TColorChannel)
    TScrollbar(TBox* attachParent, int width, int height, int top, int left, short colorPair, Orientation orientation = VERTICAL, Style style = ASCII):
        TBox(attachParent, width, height, top, left, colorPair), target(nullptr), orientation(orientation), style(style)
    {
        setScrollable(true);
    }

    // Manual-position: with a TBox* target — subscribes to scroll changes
    // without overriding the manually-set position and size.
    TScrollbar(TBox* attachParent, TBox* tTarget, int width, int height, int top, int left, short colorPair, Orientation orientation = VERTICAL, Style style = ASCII):
        TBox(attachParent, width, height, top, left, colorPair), target(tTarget), orientation(orientation), style(style)
    {
        setScrollable(true);
        // Use the ITScrollable* overload to subscribe without repositioning.
        if (tTarget) setTarget(static_cast<ITScrollable*>(tTarget));
    }

    void setTarget(TBox* t) {
        target = t;
        if (!t) return;
        if (orientation == VERTICAL) {
            setSize(1, t->getHeight());
            setPosition(t->getTop(), t->getLeft() + t->getWidth());
        } else {
            setSize(t->getWidth(), 1);
            setPosition(t->getTop() + t->getHeight(), t->getLeft());
        }
        t->onScrollChangeSubscribe([this]() {
            dirty = true;
        });
        dirty = true;
    }

    void setTarget(ITScrollable* t) {
        target = t;
        if (!t) return;
        t->onScrollChangeSubscribe([this]() {
            dirty = true;
        });
        dirty  = true;
    }

    void setStyle(const Style& s) {
        style = s;
        dirty = true;
    }

    // ── events ────────────────────────────────────────────────────

    TEventResult onMouseScroll(int /*x*/, int /*y*/, unsigned int direction) override {
        TEventResult result = TBox::onMouseScroll(/*x=*/0, /*y=*/0, direction);
        if (result & TEventResult::Stop) return result;
        if (!target) return result;
        int delta = (direction == 4) ? -1 : 1;
        target->setScrollValue(target->getScrollValue(orientation) + delta, orientation);
        dirty = true;
        return result | TEventResult::Handled;
    }

   TEventResult onMouseDown(int x, int y, unsigned int button) override {
        TEventResult result = TBox::onMouseDown(x, y, button);
        if (result & TEventResult::Stop) return result;
        if (button != 1 || !target) return result;

        int barLen = (orientation == VERTICAL) ? height : width;
        computeThumb(barLen);

        int rel = (orientation == VERTICAL)
            ? (y - getTopAbsolute())
            : (x - getLeftAbsolute());

        if (rel == 0) {
            target->setScrollValue(target->getScrollValue(orientation) - 1, orientation);
        } else if (rel == barLen - 1) {
            target->setScrollValue(target->getScrollValue(orientation) + 1, orientation);
        } else if (rel >= thumbPos + 1 && rel < thumbPos + 1 + thumbSize) {
            dragging   = true;
            dragAnchor = (orientation == VERTICAL) ? y : x;
            dragOrigin = target->getScrollValue(orientation);
        } else {
            // page scroll — jump by visible track length
            int trackLen = max(1, barLen - 2);
            int scroll   = target->getScrollValue(orientation);
            target->setScrollValue(rel < thumbPos + 1
                ? scroll - trackLen
                : scroll + trackLen, orientation);
        }

        dirty = true;
        return result | TEventResult::Handled;
    }

    TEventResult onMouseMove(int x, int y) override {
        TEventResult result = TBox::onMouseMove(x, y);
        if (result & TEventResult::Stop) return result;
        if (!dragging || !target) return result;

        int barLen   = (orientation == VERTICAL) ? height : width;
        int trackLen = max(1, barLen - 2);
        computeThumb(barLen);

        if (trackLen <= thumbSize) return result;

        int delta       = (orientation == VERTICAL) ? (y - dragAnchor) : (x - dragAnchor);
        int scrollRange = target->getScrollMax(orientation) - target->getScrollMin(orientation);
        int scrollDelta = delta * scrollRange / (trackLen - thumbSize);
        target->setScrollValue(dragOrigin + scrollDelta, orientation);

        dirty = true;
        return result | TEventResult::Handled;
    }

    TEventResult onMouseUp(int x, int y, unsigned int button) override {
        TEventResult result = TBox::onMouseUp(x, y, button);
        if (result & TEventResult::Stop) return result;
        dragging = false;
        return result | TEventResult::Handled;
    }

protected:

    void redraw() override {
        if (!target) return;

        attron(COLOR_PAIR(colorPair));

        bool isVertical = (orientation == VERTICAL);
        int  barLen     = isVertical ? height : width;

        computeThumb(barLen);

        const char* arrowStart = isVertical ? style.arrowUp : style.arrowLt;
        const char* arrowEnd   = isVertical ? style.arrowDn : style.arrowRt;
        const char* trackChar  = isVertical ? style.trackV  : style.trackH;
        const char* thumbChar  = isVertical ? style.thumbV  : style.thumbH;

        for (int i = 0; i < barLen; ++i) {
            int row = isVertical ? i : 0;
            int col = isVertical ? 0 : i;

            if (i == 0)
                print(row, col, arrowStart);
            else if (i == barLen - 1)
                print(row, col, arrowEnd);
            else {
                int  trackI  = i - 1;
                bool isThumb = (trackI >= thumbPos && trackI < thumbPos + thumbSize);
                print(row, col, isThumb ? thumbChar : trackChar);
            }
        }

        attroff(COLOR_PAIR(colorPair));
    }

private:

    void computeThumb(int barLen) {
        int trackLen    = max(1, barLen - 2);
        int scrollMin   = target->getScrollMin(orientation);
        int scrollMax   = target->getScrollMax(orientation);
        int scrollRange = scrollMax - scrollMin;

        if (scrollRange <= 0) {
            thumbPos  = 0;
            thumbSize = trackLen;
            return;
        }

        // Thumb size proportional to how much of the range is visible.
        // For a TBox this gives a thumb sized relative to content;
        // for a fixed-range ITScrollable (e.g. 0–255) the caller can
        // subclass and override getScrollMin/Max to taste.
        thumbSize = max(1, trackLen / (scrollRange + 1));
        thumbSize = min(thumbSize, trackLen);

        int scrollVal = target->getScrollValue(orientation) - scrollMin;
        thumbPos = scrollVal * (trackLen - thumbSize) / scrollRange;
        thumbPos = max(0, min(thumbPos, trackLen - thumbSize));
    }
};
