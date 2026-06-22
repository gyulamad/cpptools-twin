#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
#include "../TEventHandler.hpp"

// Testable subclass exposes protected members for testing.
class TestableEventHandler: public TEventHandler {
public:
    using TEventHandler::setFocused;
};

// ============================================================================
// TEventHandler Tests - Focus State
// ============================================================================

TEST(test_TEventHandler_initial_focus_is_false) {
    TestableEventHandler handler;
    assert(!handler.hasFocus() && "New handler should not be focused");
}

TEST(test_TEventHandler_setFocused_sets_true) {
    TestableEventHandler handler;
    handler.setFocused(true);
    assert(handler.hasFocus() && "hasFocus should return true after setFocused(true)");
}

TEST(test_TEventHandler_setFocused_sets_false) {
    TestableEventHandler handler;
    handler.setFocused(true);
    handler.setFocused(false);
    assert(!handler.hasFocus() && "hasFocus should return false after setFocused(false)");
}

// ============================================================================
// TEventHandler Tests - No handlers returns None
// ============================================================================

TEST(test_TEventHandler_onFocusGain_no_handlers_returns_none) {
    TestableEventHandler handler;
    TEventResult result = handler.onFocusGain();
    assert(!(result & TEventResult::Handled) && "Should not be handled with no callbacks");
}

TEST(test_TEventHandler_onFocusLeave_no_handlers_returns_none) {
    TestableEventHandler handler;
    TEventResult result = handler.onFocusLeave();
    assert(!(result & TEventResult::Handled) && "Should not be handled with no callbacks");
}

TEST(test_TEventHandler_onKeyPress_no_handlers_returns_none) {
    TestableEventHandler handler;
    TEventResult result = handler.onKeyPress(65, "a");
    assert(!(result & TEventResult::Handled) && "Should not be handled with no callbacks");
}

TEST(test_TEventHandler_onMouseMove_no_handlers_returns_none) {
    TestableEventHandler handler;
    TEventResult result = handler.onMouseMove(10, 20);
    assert(!(result & TEventResult::Handled) && "Should not be handled with no callbacks");
}

// ============================================================================
// TEventHandler Tests - Callback subscription and invocation
// ============================================================================

TEST(test_TEventHandler_onFocusGain_invokes_callback) {
    TestableEventHandler handler;
    handler.onFocusGainSubscribe([]() -> TEventResult {
        return TEventResult::Handled;
    });
    TEventResult result = handler.onFocusGain();
    assert(result & TEventResult::Handled && "Callback should mark event as handled");
}

TEST(test_TEventHandler_onKeyPress_invokes_callback_with_args) {
    TestableEventHandler handler;
    int capturedKey = -1;
    string capturedName;
    handler.onKeyPressSubscribe([&capturedKey, &capturedName](int key, string name) -> TEventResult {
        capturedKey = key;
        capturedName = name;
        return TEventResult::Handled;
    });
    handler.onKeyPress(65, "a");
    assert(capturedKey == 65 && "Callback should receive correct key value");
    assert(capturedName == "a" && "Callback should receive correct key name");
}

TEST(test_TEventHandler_onMouseMove_invokes_callback_with_coords) {
    TestableEventHandler handler;
    int capturedX = -1, capturedY = -1;
    handler.onMouseMoveSubscribe([&capturedX, &capturedY](int x, int y) -> TEventResult {
        capturedX = x;
        capturedY = y;
        return TEventResult::Handled;
    });
    handler.onMouseMove(30, 40);
    assert(capturedX == 30 && "Callback should receive correct X coordinate");
    assert(capturedY == 40 && "Callback should receive correct Y coordinate");
}

TEST(test_TEventHandler_onMouseClick_invokes_callback) {
    TestableEventHandler handler;
    bool called = false;
    handler.onMouseClickSubscribe([&called](int, int, unsigned int button, unsigned int repeat) -> TEventResult {
        assert(button == 1 && "Button should be 1");
        assert(repeat == 2 && "Repeat should be 2");
        called = true;
        return TEventResult::Handled;
    });
    handler.onMouseClick(10, 20, 1, 2);
    assert(called && "onMouseClick callback should have been invoked");
}

TEST(test_TEventHandler_onMouseDown_invokes_callback) {
    TestableEventHandler handler;
    bool called = false;
    handler.onMouseDownSubscribe([&called](int x, int y, unsigned int button) -> TEventResult {
        assert(x == 5 && "X should be 5");
        assert(y == 10 && "Y should be 10");
        assert(button == 2 && "Button should be 2");
        called = true;
        return TEventResult::Handled;
    });
    handler.onMouseDown(5, 10, 2);
    assert(called && "onMouseDown callback should have been invoked");
}

TEST(test_TEventHandler_onMouseUp_invokes_callback) {
    TestableEventHandler handler;
    bool called = false;
    handler.onMouseUpSubscribe([&called](int x, int y, unsigned int button) -> TEventResult {
        assert(x == 5 && "X should be 5");
        assert(y == 10 && "Y should be 10");
        assert(button == 3 && "Button should be 3");
        called = true;
        return TEventResult::Handled;
    });
    handler.onMouseUp(5, 10, 3);
    assert(called && "onMouseUp callback should have been invoked");
}

TEST(test_TEventHandler_onMouseScroll_invokes_callback) {
    TestableEventHandler handler;
    bool called = false;
    handler.onMouseScrollSubscribe([&called](int /*x*/, int /*y*/, unsigned int direction) -> TEventResult {
        assert(direction == 4 && "Direction should be 4");
        called = true;
        return TEventResult::Handled;
    });
    handler.onMouseScroll(5, 10, 4);
    assert(called && "onMouseScroll callback should have been invoked");
}

TEST(test_TEventHandler_onResize_invokes_callback) {
    TestableEventHandler handler;
    bool called = false;
    handler.onResizeSubscribe([&called](int cols, int rows) -> TEventResult {
        assert(cols == 80 && "Cols should be 80");
        assert(rows == 24 && "Rows should be 24");
        called = true;
        return TEventResult::Handled;
    });
    handler.onResize(80, 24);
    assert(called && "onResize callback should have been invoked");
}

// ============================================================================
// TEventHandler Tests - Multiple callbacks and propagation
// ============================================================================

TEST(test_TEventHandler_multiple_callbacks_all_invoked) {
    TestableEventHandler handler;
    int callCount = 0;
    handler.onKeyPressSubscribe([&callCount](int, string) -> TEventResult {
        callCount++;
        return TEventResult::None;
    });
    handler.onKeyPressSubscribe([&callCount](int, string) -> TEventResult {
        callCount++;
        return TEventResult::Handled;
    });
    handler.onKeyPress(65, "a");
    assert(callCount == 2 && "Both callbacks should have been invoked");
}

TEST(test_TEventHandler_stop_propagation_halts_callbacks) {
    TestableEventHandler handler;
    int callCount = 0;
    handler.onKeyPressSubscribe([&callCount](int, string) -> TEventResult {
        callCount++;
        return TEventResult::Stop | TEventResult::Handled;
    });
    handler.onKeyPressSubscribe([&callCount](int, string) -> TEventResult {
        callCount++;
        return TEventResult::None;
    });
    handler.onKeyPress(65, "a");
    assert(callCount == 1 && "Stop should halt further callback invocation");
}

TEST(test_TEventHandler_results_accumulate_with_or) {
    TestableEventHandler handler;
    handler.onKeyPressSubscribe([](int, string) -> TEventResult {
        return TEventResult::Handled;
    });
    handler.onKeyPressSubscribe([](int, string) -> TEventResult {
        return TEventResult::None;
    });
    TEventResult result = handler.onKeyPress(65, "a");
    assert(result & TEventResult::Handled && "Accumulated result should include Handled bit");
}

#endif
