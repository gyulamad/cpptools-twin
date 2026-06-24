#pragma once

#include <vector>
#include <functional>

using namespace std;

// -------------------------------------------------------
// ITScrollable — thin interface for anything a TScrollbar can drive
// -------------------------------------------------------

class ITScrollable {
public:
    // Orientation constants — extensible int-based identifiers.
    // TScrollbar::Orientation maps directly to these values (VERTICAL=0, HORIZONTAL=1).
    static constexpr int ORIENTATION_VERTICAL   = 0;
    static constexpr int ORIENTATION_HORIZONTAL = 1;

    virtual ~ITScrollable() {}

    // Scroll accessors — orientation parameter allows any number of axes.
    virtual int  getScrollValue(int orientation) const     = 0;
    virtual int  getScrollMin(int orientation)   const     = 0;
    virtual int  getScrollMax(int orientation)   const     = 0;
    virtual void setScrollValue(int value, int orientation) = 0;

    void onScrollChangeSubscribe(function<void()> callback) {
        scrollChangeHandlers.push_back(callback);
    }

protected:
    void notifyScrollChange() {
        for (auto& callback : scrollChangeHandlers)
            callback();
    }

private:
    vector<function<void()>> scrollChangeHandlers;
};
