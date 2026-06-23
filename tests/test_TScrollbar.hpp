#pragma once

#ifdef TEST

#include <string.h>
#include "../../misc/TEST.hpp"
#include "../TScrollbar.hpp"
#include "test_NcursesScreen.hpp"

// ============================================================================
// TScrollbar Tests - Orientation and Construction
// ============================================================================

TEST(test_TScrollbar_vertical_auto_positions_after_target) {
    NCURSES_SETUP;
    TBox target(20, 10, 5, 3, 1, vector<string>{"line1", "line2"});
    TScrollbar sb(&target, 1, TScrollbar::VERTICAL);
    // Vertical: top=target.top=5, left=target.left+target.width=3+20=23
    assert(sb.getTop() == 5 && "Vertical scrollbar should be at target's top");
    assert(sb.getLeft() == 23 && "Vertical scrollbar should be right of target");
    assert(sb.getHeight() == 10 && "Vertical scrollbar height should match target height");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_horizontal_auto_positions_below_target) {
    NCURSES_SETUP;
    TBox target(20, 10, 5, 3, 1, vector<string>{"line1", "line2"});
    TScrollbar sb(&target, 1, TScrollbar::HORIZONTAL);
    // Horizontal: top=target.top+target.height=5+10=15, left=target.left=3
    assert(sb.getTop() == 15 && "Horizontal scrollbar should be below target");
    assert(sb.getLeft() == 3 && "Horizontal scrollbar should be at target's left");
    assert(sb.getWidth() == 20 && "Horizontal scrollbar width should match target width");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_is_scrollable_by_default) {
    NCURSES_SETUP;
    TBox target(20, 10, 5, 3, 1, {"line1"});
    TScrollbar sb(&target, 1, TScrollbar::VERTICAL);
    assert(sb.isScrollable() && "TScrollbar should be scrollable by default");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_manual_position_constructor) {
    NCURSES_SETUP;
    TBox parent(40, 25, 0, 0, 1, "");
    TScrollbar sb(&parent, &parent, 1, 10, 20, 3, 1, TScrollbar::VERTICAL);
    assert(sb.getWidth() == 1 && "Width should be 1");
    assert(sb.getHeight() == 10 && "Height should be 10");
    assert(sb.getTop() == 20 && "Top should be 20");
    assert(sb.getLeft() == 3 && "Left should be 3");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_setStyle_updates_style) {
    NCURSES_SETUP;
    TBox target(20, 10, 5, 3, 1, vector<string>{"line1"});
    TScrollbar sb(&target, 1, TScrollbar::VERTICAL);
    // setStyle is public and accepts a Style struct — verify no exception on call.
    sb.setStyle(TScrollbar::ASCII);
    assert(sb.getWidth() == 1 && "Scrollbar should still be valid after style change");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_unicode_style_constant_values) {
    // Verify the UNICODE constant has expected values (compile-time check).
    string arrowUp = TScrollbar::UNICODE.arrowUp;
    assert(arrowUp == "▴" && "Default UNICODE style arrowUp should be correct");
}

TEST(test_TScrollbar_ascii_style_constant_values) {
    // Verify the ASCII constant has expected values (compile-time check).
    string arrowUp = TScrollbar::ASCII.arrowUp;
    assert(arrowUp == "^" && "Default ASCII style arrowUp should be correct");
}

// ============================================================================
// TScrollbar Tests - Scroll interaction with target
// ============================================================================

TEST(test_TScrollbar_onMouseScroll_changes_target_scroll) {
    NCURSES_SETUP;
    TBox target(10, 3, 5, 3, 1, {"line1", "line2", "line3", "line4"});
    TScrollbar sb(&target, 1, TScrollbar::VERTICAL);
    int initialScroll = target.getScrollTop();
    // direction 5 = wheel down -> increment scroll
    sb.onMouseScroll(0, 0, 5);
    assert(target.getScrollTop() == initialScroll + 1 && "Wheel down should increment scrollTop by 1");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_onMouseScroll_up_decrements) {
    NCURSES_SETUP;
    TBox target(10, 3, 5, 3, 1, {"line1", "line2", "line3", "line4"});
    // Set scroll to middle first
    target.setScrollTop(1);
    TScrollbar sb(&target, 1, TScrollbar::VERTICAL);
    // direction 4 = wheel up -> decrement scroll
    sb.onMouseScroll(0, 0, 4);
    assert(target.getScrollTop() == 0 && "Wheel up should decrement scrollTop by 1");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_onMouseDown_arrow_up_decrements) {
    NCURSES_SETUP;
    // Use more lines than height so scrolling is possible (maxScroll = 8-5=3)
    TBox target(10, 5, 5, 3, 1, vector<string>{"l1","l2","l3","l4","l5","l6","l7","l8"});
    target.setScrollTop(2);
    assert(target.getScrollTop() == 2 && "scrollTop should be settable to 2");
    TScrollbar sb(&target, 1, TScrollbar::VERTICAL);
    // Click at relative position 0 (arrow up) - absolute y = getTopAbsolute() + 0
    int absY = sb.getTopAbsolute();
    sb.onMouseDown(sb.getLeftAbsolute(), absY, 1);
    assert(target.getScrollTop() == 1 && "Clicking arrow up should decrement scroll");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_onMouseUp_clears_dragging) {
    NCURSES_SETUP;
    TBox target(20, 10, 5, 3, 1, {"line1"});
    TScrollbar sb(&target, 1, TScrollbar::VERTICAL);
    // Simulate drag start then release
    int absY = sb.getTopAbsolute() + 2;
    sb.onMouseDown(sb.getLeftAbsolute(), absY, 1);
    TEventResult result = sb.onMouseUp(0, 0, 1);
    assert(result & TEventResult::Handled && "onMouseUp should return Handled");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TScrollbar Tests - onMouseClick (click event handling)
// ============================================================================

TEST(test_TScrollbar_onMouseClick_arrow_up_decrements) {
    NCURSES_SETUP;
    // Use more lines than height so scrolling is possible (maxScroll = 8-5=3)
    TBox target(10, 5, 5, 3, 1, vector<string>{"l1","l2","l3","l4","l5","l6","l7","l8"});
    target.setScrollTop(2);
    TScrollbar sb(&target, 1, TScrollbar::VERTICAL);
    // Click at relative position 0 (arrow up) — absolute y = getTopAbsolute() + 0
    int absY = sb.getTopAbsolute();
    sb.onMouseClick(sb.getLeftAbsolute(), absY, 1, 1);
    assert(target.getScrollTop() == 1 && "onMouseClick on arrow up should decrement scroll by 1");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_onMouseClick_arrow_down_increments) {
    NCURSES_SETUP;
    TBox target(10, 5, 5, 3, 1, vector<string>{"l1","l2","l3","l4","l5","l6","l7","l8"});
    target.setScrollTop(2);
    TScrollbar sb(&target, 1, TScrollbar::VERTICAL);
    // Click at relative position barLen-1 (arrow down) — absolute y = getTopAbsolute() + height - 1
    int absY = sb.getTopAbsolute() + sb.getHeight() - 1;
    sb.onMouseClick(sb.getLeftAbsolute(), absY, 1, 1);
    assert(target.getScrollTop() == 3 && "onMouseClick on arrow down should increment scroll by 1");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_onMouseClick_horizontal_arrow_left_decrements) {
    NCURSES_SETUP;
    // Content wider than box so horizontal scrolling is possible (right=30, maxScroll=10).
    TBox target(20, 5, 5, 3, 1, vector<string>{"this_is_a_very_long_line_of_text_for_testing"});
    target.setScrollLeft(2);
    TScrollbar sb(&target, 1, TScrollbar::HORIZONTAL);
    // Click at relative position 0 (left arrow) — absolute x = getLeftAbsolute() + 0
    int absX = sb.getLeftAbsolute();
    sb.onMouseClick(absX, sb.getTopAbsolute(), 1, 1);
    assert(target.getScrollLeft() == 1 && "onMouseClick on left arrow should decrement scroll by 1");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_onMouseClick_horizontal_arrow_right_increments) {
    NCURSES_SETUP;
    // Content wider than box so horizontal scrolling is possible (right=30, maxScroll=10).
    TBox target(20, 5, 5, 3, 1, vector<string>{"this_is_a_very_long_line_of_text_for_testing"});
    target.setScrollLeft(2);
    TScrollbar sb(&target, 1, TScrollbar::HORIZONTAL);
    // Click at relative position barLen-1 (right arrow) — absolute x = getLeftAbsolute() + width - 1
    int absX = sb.getLeftAbsolute() + sb.getWidth() - 1;
    sb.onMouseClick(absX, sb.getTopAbsolute(), 1, 1);
    assert(target.getScrollLeft() == 3 && "onMouseClick on right arrow should increment scroll by 1");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_onMouseClick_returns_handled) {
    NCURSES_SETUP;
    TBox target(20, 5, 5, 3, 1, vector<string>{"l1","l2","l3"});
    TScrollbar sb(&target, 1, TScrollbar::HORIZONTAL);
    int absX = sb.getLeftAbsolute();
    TEventResult result = sb.onMouseClick(absX, sb.getTopAbsolute(), 1, 1);
    assert(result & TEventResult::Handled && "onMouseClick should return Handled");
    NCURSES_TEARDOWN;
}

TEST(test_TScrollbar_onMouseClick_wrong_button_no_effect) {
    NCURSES_SETUP;
    // Content wider than box so horizontal scrolling is possible (right=30, maxScroll=10).
    TBox target(20, 5, 5, 3, 1, vector<string>{"this_is_a_very_long_line_of_text_for_testing"});
    target.setScrollLeft(2);
    TScrollbar sb(&target, 1, TScrollbar::HORIZONTAL);
    int absX = sb.getLeftAbsolute();
    // Button 2 (middle click) should not change scroll value.
    sb.onMouseClick(absX, sb.getTopAbsolute(), 2, 1);
    assert(target.getScrollLeft() == 2 && "Non-button-1 click should not affect scroll");
    NCURSES_TEARDOWN;
}

#endif