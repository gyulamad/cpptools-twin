#pragma once

#include <vector>
#include <functional>

using namespace std;

// -------------------------------------------------------
// ITScrollable — thin interface for anything a TScrollbar can drive
// -------------------------------------------------------

class ITScrollable {
public:
    virtual int  getScrollValue() const    = 0;
    virtual int  getScrollMin()   const    = 0;
    virtual int  getScrollMax()   const    = 0;
    virtual void setScrollValue(int value) = 0;

    void onScrollChangeSubscribe(function<void()> callback) {
        scrollChangeHandlers.push_back(callback);
    }

    virtual ~ITScrollable() {}

protected:
    void notifyScrollChange() {
        for (auto& cb : scrollChangeHandlers)
            cb();
    }

private:
    vector<function<void()>> scrollChangeHandlers;
};
