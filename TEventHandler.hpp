#pragma once

#include <string>
#include <vector>
#include <functional>

#include "TEventResult.hpp"

using namespace std;

/* TEventHandler — Event propagation via bitmask callbacks.
 *
 * WHY TWO BITS INSTEAD OF A BOOL?
 * A plain bool can only express one dimension: true/false.  But event handlers
 * need to communicate two independent pieces of information:
 *   (1) Did this handler consume/process the event?          → Handled bit
 *   (2) Should propagation stop here or continue downstream? → Stop bit
 *
 * With a bool, "handled + stop" and "unhandled + stop" both collapse to the
 * same value, so the parent can never tell whether the child actually did
 * something.  The bitmask separates these concerns:
 *
 *   TEventResult::None          — unhandled, continue chain (default)
 *   TEventResult::Handled       — consumed but let others see it too
 *   TEventResult::Stop          — force-stop without handling (emergency)
 *   Handled | Stop              — consumed AND stop propagation
 *
 * Checking is always a single operation:  if (result & TEventResult::Stop) ...
 */

class TEventHandler {
protected:
    // EventHandlers allows to subscribe to events without extending the base class.
    // However, as per convention any built-in library functionality overrides.
    // Child classes should call the parent TEventHandler methods when overriding.

    vector<function<TEventResult(void)>> focusGainHandlers;
    vector<function<TEventResult(void)>> focusLeaveHandlers;
    bool focused = false;
    vector<function<TEventResult(int, string)>> keyPressHandlers;
    vector<function<TEventResult(int, int)>> mouseMoveHandlers;
    vector<function<TEventResult(int, int, unsigned int, unsigned int)>> mouseClickHandlers;
    vector<function<TEventResult(int, int, unsigned int)>> mouseDownHandlers;
    vector<function<TEventResult(int, int, unsigned int)>> mouseUpHandlers;
    vector<function<TEventResult(int, int, unsigned int)>> mouseScrollHandlers;
    vector<function<TEventResult(int, int)>> resizeHandlers;

public:
    TEventHandler() {}
    virtual ~TEventHandler() {}

    bool hasFocus() const { return focused; }

    void onFocusGainSubscribe(function<TEventResult(void)> callback) {
        focusGainHandlers.push_back(callback);
    }

    void onFocusLeaveSubscribe(function<TEventResult(void)> callback) {
        focusLeaveHandlers.push_back(callback);
    }

    void onKeyPressSubscribe(function<TEventResult(int, string)> callback) {
        keyPressHandlers.push_back(callback);
    }

    void onMouseMoveSubscribe(function<TEventResult(int, int)> callback) {
        mouseMoveHandlers.push_back(callback);
    }

    void onMouseClickSubscribe(function<TEventResult(int, int, unsigned int, unsigned int)> callback) {
        mouseClickHandlers.push_back(callback);
    }

    void onMouseDownSubscribe(function<TEventResult(int, int, unsigned int)> callback) {
        mouseDownHandlers.push_back(callback);
    }

    void onMouseUpSubscribe(function<TEventResult(int, int, unsigned int)> callback) {
        mouseUpHandlers.push_back(callback);
    }

    void onMouseScrollSubscribe(function<TEventResult(int, int, unsigned int)> callback) {
        mouseScrollHandlers.push_back(callback);
    }

    void onResizeSubscribe(function<TEventResult(int, int)> callback) {
        resizeHandlers.push_back(callback);
    }

protected:
    // Called by TWindow::setFocus to update internal state before events fire
    void setFocused(bool value) { focused = value; }

public:
    friend class TWindow;

    virtual TEventResult onFocusGain() {
        TEventResult results = TEventResult::None;
        for (auto& callback : focusGainHandlers) {
            TEventResult result = callback();
            if (result & TEventResult::Stop) return result;
            results |= result;
        }
        return results;
    }

    virtual TEventResult onFocusLeave() {
        TEventResult results = TEventResult::None;
        for (auto& callback : focusLeaveHandlers) {
            TEventResult result = callback();
            if (result & TEventResult::Stop) return result;
            results |= result;
        }
        return results;
    }

    virtual TEventResult onKeyPress(int key, string name) {
        TEventResult results = TEventResult::None;
        for (auto& callback : keyPressHandlers) {
            TEventResult result = callback(key, name);
            if (result & TEventResult::Stop) return result;
            results |= result;
        }
        return results;
    }

    virtual TEventResult onMouseMove(int x, int y) {
        TEventResult results = TEventResult::None;
        for (auto& callback : mouseMoveHandlers) {
            TEventResult result = callback(x, y);
            if (result & TEventResult::Stop) return result;
            results |= result;
        }
        return results;
    }

    virtual TEventResult onMouseClick(int x, int y, unsigned int button, unsigned int repeat) {
        TEventResult results = TEventResult::None;
        for (auto& callback : mouseClickHandlers) {
            TEventResult result = callback(x, y, button, repeat);
            if (result & TEventResult::Stop) return result;
            results |= result;
        }
        return results;
    }

    virtual TEventResult onMouseDown(int x, int y, unsigned int button) {
        TEventResult results = TEventResult::None;
        for (auto& callback : mouseDownHandlers) {
            TEventResult result = callback(x, y, button);
            if (result & TEventResult::Stop) return result;
            results |= result;
        }
        return results;
    }

    virtual TEventResult onMouseUp(int x, int y, unsigned int button) {
        TEventResult results = TEventResult::None;
        for (auto& callback : mouseUpHandlers) {
            TEventResult result = callback(x, y, button);
            if (result & TEventResult::Stop) return result;
            results |= result;
        }
        return results;
    }

    virtual TEventResult onMouseScroll(int x, int y, unsigned int direction) {
        TEventResult results = TEventResult::None;
        for (auto& callback : mouseScrollHandlers) {
            TEventResult result = callback(x, y, direction);
            if (result & TEventResult::Stop) return result;
            results |= result;
        }
        return results;
    }

    virtual TEventResult onResize(int cols, int rows) {
        TEventResult results = TEventResult::None;
        for (auto& callback : resizeHandlers) {
            TEventResult result = callback(cols, rows);
            if (result & TEventResult::Stop) return result;
            results |= result;
        }
        return results;
    }

};
