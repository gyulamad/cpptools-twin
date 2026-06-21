#pragma once

#include <string>
#include <vector>
#include <functional>

using namespace std;

class TEventHandler {
protected:
    
    // EventHandlers allows to subscribe to events without extending the base class
    // however, as per convention any built in library functionalitie overrides.
    // Child class should call the parent TEventHandler methods when override.

    vector<function<bool(void)>> focusGainHandlers;
    vector<function<bool(void)>> focusLeaveHandlers;
    bool focused = false;
    vector<function<bool(int, string)>> keyPressHandlers;
    vector<function<bool(int, int)>> mouseMoveHandlers;
    vector<function<bool(int, int, unsigned int, unsigned int)>> mouseClickHandlers;
    vector<function<bool(int, int, unsigned int)>> mouseDownHandlers;
    vector<function<bool(int, int, unsigned int)>> mouseUpHandlers;
    vector<function<bool(int, int, unsigned int)>> mouseScrollHandlers;
    vector<function<bool(int, int)>> resizeHandlers;

public:
    TEventHandler() {}
    virtual ~TEventHandler() {}

    bool hasFocus() const { return focused; }

    void onFocusGainSubscribe(function<bool(void)> callback) {
        focusGainHandlers.push_back(callback);
    }

    void onFocusLeaveSubscribe(function<bool(void)> callback) {
        focusLeaveHandlers.push_back(callback);
    }

    void onKeyPressSubscribe(function<bool(int, string)> callback) {
        keyPressHandlers.push_back(callback);
    }

    void onMouseMoveSubscribe(function<bool(int, int)> callback) {
        mouseMoveHandlers.push_back(callback);
    }

    void onMouseClickSubscribe(function<bool(int, int, unsigned int, unsigned int)> callback) {
        mouseClickHandlers.push_back(callback);
    }

    void onMouseDownSubscribe(function<bool(int, int, unsigned int)> callback) {
        mouseDownHandlers.push_back(callback);
    }

    void onMouseUpSubscribe(function<bool(int, int, unsigned int)> callback) {
        mouseUpHandlers.push_back(callback);
    }

    void onMouseScrollSubscribe(function<bool(int, int, unsigned int)> callback) {
        mouseScrollHandlers.push_back(callback);
    }

    void onResizeSubscribe(function<bool(int, int)> callback) {
        resizeHandlers.push_back(callback);
    }

protected:
    // Called by TWindow::setFocus to update internal state before events fire
    void setFocused(bool value) { focused = value; }

public:
    friend class TWindow;

    virtual void onFocusGain() {
        for (auto callback: focusGainHandlers)
            if (!callback()) return;
    }

    virtual void onFocusLeave() {
        for (auto callback: focusLeaveHandlers)
            if (!callback()) return;
    }

    virtual bool onKeyPress(int key, string name) {
        for (auto callback: keyPressHandlers)
            if (!callback(key, name)) return false;
        return true;
    }

    virtual bool onMouseMove(int x, int y) {
        for (auto callback: mouseMoveHandlers)
            if (!callback(x, y)) return false;
        return true;
    }

    virtual bool onMouseClick(int x, int y, unsigned int button, unsigned int repeat) {
        for (auto callback: mouseClickHandlers)
            if (!callback(x, y, button, repeat)) return false;
        return true;
    }

    virtual bool onMouseDown(int x, int y, unsigned int button) {
        for (auto callback: mouseDownHandlers)
            if (!callback(x, y, button)) return false;
        return true;
    }

    virtual bool onMouseUp(int x, int y, unsigned int button) {
        for (auto callback: mouseUpHandlers)
            if (!callback(x, y, button)) return false;
        return true;
    }

    virtual bool onMouseScroll(int x, int y, unsigned int direction) {
        for (auto callback: mouseScrollHandlers)
            if (!callback(x, y, direction)) return false;
        return true;
    }

    virtual bool onResize(int cols, int rows) {
        for (auto callback: resizeHandlers)
            if (!callback(cols, rows)) return false;
        return true;
    }

};
