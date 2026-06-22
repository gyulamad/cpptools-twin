#pragma once

#ifdef TEST

#include <string.h>
#include "../../misc/TEST.hpp"
#include "../TInput.hpp"
#include "test_NcursesScreen.hpp"

// Testable subclass exposes protected event handlers for testing.
class TestableInput: public TInput {
public:
    using TInput::TInput;
    using TInput::onKeyPress;
    using TInput::onMouseClick;
};

// ============================================================================
// TInput Tests - Constructor and Properties
// ============================================================================

TEST(test_TInput_cursor_initially_at_zero) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    assert(input.getCursorRow() == 0 && "cursorRow should start at 0");
    assert(input.getCursorCol() == 0 && "cursorCol should start at 0");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_single_line_default_multiline_false) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    assert(!input.getMultiLine() && "Single line input should have multiLine=false");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_multi_line_default_multiline_true) {
    NCURSES_SETUP;
    TestableInput input(20, 3, 1);
    assert(input.getMultiLine() && "Height > 1 should set multiLine=true");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_setMultiLine_updates_flag) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    input.setMultiLine(true);
    assert(input.getMultiLine() && "setMultiLine(true) should update the flag");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_setSize_updates_multiline) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    assert(!input.getMultiLine() && "Should start as single-line");
    input.setSize(30, 5);
    assert(input.getMultiLine() && "setSize with height>1 should enable multiLine");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TInput Tests - Text Input (onKeyPress)
// ============================================================================

TEST(test_TInput_inserts_printable_char) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    input.onKeyPress('H', "H");
    assert(input.getContents() == "H" && "Should insert 'H' at cursor");
    assert(input.getCursorCol() == 1 && "cursorCol should advance to 1");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_inserts_multiple_chars) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    input.onKeyPress('H', "H");
    input.onKeyPress('i', "i");
    assert(input.getContents() == "Hi" && "Should insert 'Hi'");
    assert(input.getCursorCol() == 2 && "cursorCol should be at 2");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_backspace_deletes_char) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    input.onKeyPress('A', "A");
    input.onKeyPress('B', "B");
    assert(input.getContents() == "AB" && "Content should be 'AB'");
    input.onKeyPress(KEY_BACKSPACE, "Backspace");
    assert(input.getContents() == "A" && "After backspace content should be 'A'");
    assert(input.getCursorCol() == 1 && "cursorCol should move to 1");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_backspace_at_start_does_nothing) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    assert(input.getCursorCol() == 0 && "cursor should be at start");
    input.onKeyPress(KEY_BACKSPACE, "Backspace");
    assert(input.getContents().empty() && "Content should remain empty after backspace at start");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_delete_key_removes_char_at_cursor) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    input.onKeyPress('A', "A");
    input.onKeyPress('B', "B");
    // Move cursor back to between A and B
    input.onKeyPress(KEY_LEFT, "Left");
    assert(input.getCursorCol() == 1 && "cursor should be at position 1");
    input.onKeyPress(KEY_DC, "Delete");
    assert(input.getContents() == "A" && "After delete content should be 'A'");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TInput Tests - Cursor Movement
// ============================================================================

TEST(test_TInput_cursor_right_moves_forward) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    input.onKeyPress('A', "A");
    assert(input.getCursorCol() == 1 && "cursor should be at 1 after typing A");
    input.onKeyPress(KEY_RIGHT, "Right");
    int col = input.getCursorCol();
    assert((col == 2 || col == 1) && "cursor should move right or stay at end");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_cursor_left_moves_backward) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    input.onKeyPress('A', "A");
    assert(input.getCursorCol() == 1 && "cursor should be at 1 after typing A");
    input.onKeyPress(KEY_LEFT, "Left");
    assert(input.getCursorCol() == 0 && "cursor should move left to 0");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_cursor_left_at_start_stays) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    assert(input.getCursorCol() == 0 && "cursor should start at 0");
    input.onKeyPress(KEY_LEFT, "Left");
    assert(input.getCursorCol() == 0 && "cursor should stay at 0 when already at left edge");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_home_moves_to_line_start) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    input.onKeyPress('A', "A");
    input.onKeyPress('B', "B");
    assert(input.getCursorCol() == 2 && "cursor should be at 2 after typing AB");
    input.onKeyPress(KEY_HOME, "Home");
    assert(input.getCursorCol() == 0 && "Home should move cursor to column 0");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_end_moves_to_line_end) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    input.onKeyPress('A', "A");
    assert(input.getCursorCol() == 1 && "cursor should be at 1 after typing A");
    input.onKeyPress(KEY_LEFT, "Left");
    assert(input.getCursorCol() == 0 && "cursor moved back to 0");
    input.onKeyPress(KEY_END, "End");
    assert(input.getCursorCol() == 1 && "End should move cursor to end of line (col 1)");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TInput Tests - Multi-line Operations
// ============================================================================

TEST(test_TInput_newline_splits_line_in_multiline_mode) {
    NCURSES_SETUP;
    TestableInput input(20, 3, 1);
    assert(input.getMultiLine() && "Should be multi-line");
    input.onKeyPress('A', "A");
    input.onKeyPress('\n', "Enter");
    input.onKeyPress('B', "B");
    string contents = input.getContents();
    assert(contents == "A\nB" && "Newline should split into two lines: 'A\\nB'");
    assert(input.getCursorRow() == 1 && "cursorRow should be on line 1 (second line)");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_cursor_up_moves_to_previous_line) {
    NCURSES_SETUP;
    TestableInput input(20, 3, 1);
    input.onKeyPress('A', "A");
    input.onKeyPress('\n', "Enter");
    input.onKeyPress('B', "B");
    assert(input.getCursorRow() == 1 && "cursor should be on row 1");
    input.onKeyPress(KEY_UP, "Up");
    assert(input.getCursorRow() == 0 && "cursor should move up to row 0");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_cursor_down_moves_to_next_line) {
    NCURSES_SETUP;
    TestableInput input(20, 3, 1);
    assert(input.getCursorRow() == 0 && "cursor should start at row 0");
    // First ensure a second line exists by inserting newline
    input.onKeyPress('\n', "Enter");
    assert(input.getCursorRow() == 1 && "After newline cursor is on row 1");
    input.onKeyPress(KEY_UP, "Up");
    assert(input.getCursorRow() == 0 && "cursor moved up to row 0");
    input.onKeyPress(KEY_DOWN, "Down");
    assert(input.getCursorRow() == 1 && "cursor should move down to row 1");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_backspace_at_start_of_line_merges_with_previous) {
    NCURSES_SETUP;
    TestableInput input(20, 3, 1);
    input.onKeyPress('A', "A");
    input.onKeyPress('\n', "Enter");
    input.onKeyPress('B', "B");
    // cursor is at row=1 col=1 (after 'B')
    input.onKeyPress(KEY_LEFT, "Left");
    assert(input.getCursorCol() == 0 && "cursor should be at start of line 2");
    input.onKeyPress(KEY_BACKSPACE, "Backspace");
    string contents = input.getContents();
    assert(contents == "AB" && "Backspace at start of line should merge: 'A' + 'B' = 'AB'");
    assert(input.getCursorRow() == 0 && "cursor should move to previous row (row 0)");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_delete_at_end_of_line_merges_with_next) {
    NCURSES_SETUP;
    TestableInput input(20, 3, 1);
    input.onKeyPress('A', "A");
    input.onKeyPress('\n', "Enter");
    input.onKeyPress('B', "B");
    // Move cursor to end of first line (row=0 col=1)
    input.onKeyPress(KEY_UP, "Up");
    assert(input.getCursorRow() == 0 && "cursor should be on row 0");
    assert(input.getCursorCol() == 1 && "cursor should be at end of 'A'");
    input.onKeyPress(KEY_DC, "Delete");
    string contents = input.getContents();
    assert(contents == "AB" && "Delete at end of line should merge with next: 'A' + 'B' = 'AB'");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TInput Tests - Tab inserts spaces
// ============================================================================

TEST(test_TInput_tab_inserts_four_spaces) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    input.onKeyPress('\t', "Tab");
    assert(input.getContents() == "    " && "Tab should insert four spaces");
    assert(input.getCursorCol() == 4 && "cursorCol should advance by 4");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TInput Tests - Event handling returns Handled
// ============================================================================

TEST(test_TInput_onKeyPress_printable_returns_handled) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 1);
    TEventResult result = input.onKeyPress('A', "A");
    assert(result & TEventResult::Handled && "Printable key should return Handled");
    NCURSES_TEARDOWN;
}

TEST(test_TInput_onMouseClick_returns_handled) {
    NCURSES_SETUP;
    TestableInput input(20, 1, 5, 3, 1);
    // Click inside the input area (absolute coords: x=8, y=6 -> relative col=5, row=1)
    int absX = input.getLeftAbsolute() + 5;
    int absY = input.getTopAbsolute();
    TEventResult result = input.onMouseClick(absX, absY, 1, 1);
    assert(result & TEventResult::Handled && "onMouseClick should return Handled");
    NCURSES_TEARDOWN;
}

#endif
