#pragma once

#ifdef TEST

#include <string.h>
#include "../../misc/TEST.hpp"
#include "../TWindow.hpp"

// ============================================================================
// TWindow Tests - Focus management (requires ncurses init via newterm/delscreen)
// ============================================================================

// ============================================================================
// TWindow Tests - Focus management (requires ncurses init via newterm/delscreen)
// ============================================================================

TEST(test_TWindow_initial_focus_is_nullptr) {
    setenv("TERM", "xterm-256color", 1);
    FILE* devnull = fopen("/dev/null", "w");
    SCREEN* screen = newterm("xterm-256color", devnull, stdin);
    TWindow window;
    assert(window.getFocus() == nullptr && "Initial focus should be nullptr");
    delscreen(screen);
}

TEST(test_TWindow_setFocus_sets_and_gets_focus) {
    setenv("TERM", "xterm-256color", 1);
    FILE* devnull = fopen("/dev/null", "w");
    SCREEN* screen = newterm("xterm-256color", devnull, stdin);
    TWindow window;
    TBox box(10, 3, 1, "test");
    window.setFocus(&box);
    assert(window.getFocus() == &box && "getFocus should return the set focus target");
    delscreen(screen);
}

TEST(test_TWindow_setFocus_triggers_focus_events) {
    setenv("TERM", "xterm-256color", 1);
    FILE* devnull = fopen("/dev/null", "w");
    SCREEN* screen = newterm("xterm-256color", devnull, stdin);
    TWindow window;
    bool focusGained = false;
    bool focusLeft = false;
    TBox box1(10, 3, 1, "box1");
    TBox box2(10, 3, 1, "box2");

    box1.onFocusGainSubscribe([&focusGained]() -> TEventResult {
        focusGained = true;
        return TEventResult::None;
    });
    box1.onFocusLeaveSubscribe([&focusLeft]() -> TEventResult {
        focusLeft = true;
        return TEventResult::None;
    });

    window.setFocus(&box1);
    assert(focusGained && "onFocusGain should be called when setting focus");
    assert(!focusLeft && "onFocusLeave should not fire on first focus set");

    window.setFocus(&box2);
    assert(focusLeft && "onFocusLeave should be called when focus moves away");

    delscreen(screen);
}

TEST(test_TWindow_setFocus_same_box_is_noop) {
    setenv("TERM", "xterm-256color", 1);
    FILE* devnull = fopen("/dev/null", "w");
    SCREEN* screen = newterm("xterm-256color", devnull, stdin);
    TWindow window;
    int leaveCount = 0;
    TBox box(10, 3, 1, "test");
    box.onFocusLeaveSubscribe([&leaveCount]() -> TEventResult {
        leaveCount++;
        return TEventResult::None;
    });

    window.setFocus(&box);
    window.setFocus(&box); // Same box - should be a no-op
    assert(leaveCount == 0 && "Setting focus to the same box should not trigger events");

    delscreen(screen);
}

TEST(test_TWindow_getRoot_returns_root_box) {
    setenv("TERM", "xterm-256color", 1);
    FILE* devnull = fopen("/dev/null", "w");
    SCREEN* screen = newterm("xterm-256color", devnull, stdin);
    TWindow window;
    assert(window.getRoot() != nullptr && "getRoot should return a non-null root box");
    delscreen(screen);
}

TEST(test_TWindow_getTheme_returns_theme) {
    setenv("TERM", "xterm-256color", 1);
    FILE* devnull = fopen("/dev/null", "w");
    SCREEN* screen = newterm("xterm-256color", devnull, stdin);
    TWindow window;
    assert(window.getTheme() != nullptr && "getTheme should return a non-null theme");
    delscreen(screen);
}

#endif