#pragma once

#ifdef TEST

#include <string.h>
#include "../../misc/TEST.hpp"
#include "../TBox.hpp"
#include "test_NcursesScreen.hpp"

// ============================================================================
// TBox Tests - Geometry and Positioning
// ============================================================================

TEST(test_TBox_getWidth_returns_set_width) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    assert(box.getWidth() == 20 && "Width should be 20");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_getHeight_returns_set_height) {
    NCURSES_SETUP;
    TBox box(20, 15, 1, "hello");
    assert(box.getHeight() == 15 && "Height should be 15");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_getTop_returns_set_top) {
    NCURSES_SETUP;
    TBox box(20, 10, 5, 3, 1, "hello");
    assert(box.getTop() == 5 && "Top should be 5");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_getLeft_returns_set_left) {
    NCURSES_SETUP;
    TBox box(20, 10, 5, 3, 1, "hello");
    assert(box.getLeft() == 3 && "Left should be 3");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setPosition_updates_top_and_left) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    box.setPosition(7, 9);
    assert(box.getTop() == 7 && "Top should be updated to 7");
    assert(box.getLeft() == 9 && "Left should be updated to 9");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setSize_updates_dimensions) {
    NCURSES_SETUP;
    TBox box(10, 5, 1, "hello");
    box.setSize(30, 20);
    assert(box.getWidth() == 30 && "Width should be updated to 30");
    assert(box.getHeight() == 20 && "Height should be updated to 20");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_getContents_returns_raw_text) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, vector<string>{"line1", "line2"});
    string contents = box.getContents();
    assert(contents == "line1\nline2" && "getContents should join with newlines");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setContents_updates_text) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, vector<string>{"old"});
    box.setContents(vector<string>{"new1", "new2", "new3"});
    string contents = box.getContents();
    assert(contents == "new1\nnew2\nnew3" && "setContents should update the text");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setContents_with_string) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, vector<string>{"old"});
    box.setContents("hello\nworld");
    string contents = box.getContents();
    assert(contents == "hello\nworld" && "setContents with string should split on newlines");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setContents_empty_vector) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, vector<string>{"old"});
    box.setContents(vector<string>{});
    string contents = box.getContents();
    assert(contents.empty() && "Empty vector should produce empty content");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - Parent and Children
// ============================================================================

TEST(test_TBox_getParent_returns_set_parent) {
    NCURSES_SETUP;
    TBox parent(40, 20, 1, "");
    TBox child(&parent, 10, 5, 1, "child");
    assert(child.getParent() == &parent && "getParent should return the parent box");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_no_parent_is_nullptr) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    assert(box.getParent() == nullptr && "Standalone box should have null parent");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_addChild_links_parent) {
    NCURSES_SETUP;
    TBox parent(40, 20, 1, "");
    TBox child(10, 5, 1, "child");
    parent.addChild(&child);
    assert(child.getParent() == &parent && "addChild should set the parent reference");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - Scroll
// ============================================================================

TEST(test_TBox_scrollTop_initially_zero) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    assert(box.getScrollTop() == 0 && "scrollTop should start at 0");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_scrollLeft_initially_zero) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    assert(box.getScrollLeft() == 0 && "scrollLeft should start at 0");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setScrollTop_clamps_to_max) {
    NCURSES_SETUP;
    TBox box(10, 5, 1, vector<string>{"line1", "line2", "line3", "line4", "line5"});
    // bottom=5, height=5 -> max scroll = 0
    box.setScrollTop(10);
    assert(box.getScrollTop() == 0 && "scrollTop should clamp to max (0)");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setScrollTop_clamps_to_zero_min) {
    NCURSES_SETUP;
    TBox box(10, 5, 1, vector<string>{"line1", "line2"});
    box.setScrollTop(-3);
    assert(box.getScrollTop() == 0 && "scrollTop should clamp to minimum (0)");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_scrollToBottom_sets_max_scroll) {
    NCURSES_SETUP;
    TBox box(10, 3, 1, vector<string>{"line1", "line2", "line3", "line4", "line5"});
    // bottom=5, height=3 -> max scroll = 2
    box.scrollToBottom();
    assert(box.getScrollTop() == 2 && "scrollToBottom should set scrollTop to max");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_isScrollAtBottom_when_at_max) {
    NCURSES_SETUP;
    TBox box(10, 3, 1, vector<string>{"line1", "line2", "line3", "line4", "line5"});
    // bottom=5, height=3 -> max scroll = 2
    box.setScrollTop(2);
    assert(box.isScrollAtBottom() && "Should be at bottom when scrollTop equals max");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_isScrollAtBottom_when_not_at_max) {
    NCURSES_SETUP;
    TBox box(10, 3, 1, vector<string>{"line1", "line2", "line3", "line4", "line5"});
    box.setScrollTop(0);
    assert(!box.isScrollAtBottom() && "Should not be at bottom when scrollTop is 0");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_scrollable_default_is_false) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    assert(!box.isScrollable() && "scrollable should default to false");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setScrollable_sets_flag) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    box.setScrollable(true);
    assert(box.isScrollable() && "setScrollable(true) should set the flag");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - Auto Scroll and Auto Grow
// ============================================================================

TEST(test_TBox_autoScroll_default_is_false) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    assert(!box.isAutoScroll() && "autoScroll should default to false");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setAutoScroll_sets_flag) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    box.setAutoScroll(true);
    assert(box.isAutoScroll() && "setAutoScroll(true) should set the flag");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_autoGrow_default_is_false) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    assert(!box.isAutoGrow() && "autoGrow should default to false");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setAutoGrow_sets_flag) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    box.setAutoGrow(true);
    assert(box.isAutoGrow() && "setAutoGrow(true) should set the flag");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - Wrap Text
// ============================================================================

TEST(test_TBox_wrapText_default_is_false) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    assert(!box.isWrapText() && "wrapText should default to false");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setWrapText_sets_flag) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello");
    box.setWrapText(true);
    assert(box.isWrapText() && "setWrapText(true) should set the flag");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - Hit Testing (contains)
// ============================================================================

TEST(test_TBox_contains_point_inside_bounds) {
    NCURSES_SETUP;
    TBox box(20, 10, 5, 3, 1, "hello");
    // absTop=5, absLeft=3 -> point at (7, 8) should be inside
    assert(box.contains(7, 8) && "Point inside bounds should return true");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_contains_point_outside_left) {
    NCURSES_SETUP;
    TBox box(20, 10, 5, 3, 1, "hello");
    // absLeft=3 -> x=2 is outside left
    assert(!box.contains(2, 8) && "Point to the left should return false");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_contains_point_outside_right) {
    NCURSES_SETUP;
    TBox box(20, 10, 5, 3, 1, "hello");
    // absLeft=3, width=20 -> x=23 is outside right (right edge = 23 exclusive)
    assert(!box.contains(23, 8) && "Point at the right edge should return false");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_contains_point_outside_top) {
    NCURSES_SETUP;
    TBox box(20, 10, 5, 3, 1, "hello");
    // absTop=5 -> y=4 is outside top
    assert(!box.contains(7, 4) && "Point above should return false");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_contains_point_outside_bottom) {
    NCURSES_SETUP;
    TBox box(20, 10, 5, 3, 1, "hello");
    // absTop=5, height=10 -> y=15 is outside bottom (bottom edge = 15 exclusive)
    assert(!box.contains(7, 15) && "Point at the bottom edge should return false");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - findMouseAt
// ============================================================================

TEST(test_TBox_findMouseAt_returns_this_when_hit) {
    NCURSES_SETUP;
    TBox box(20, 10, 5, 3, 1, "hello");
    TBox* hit = box.findMouseAt(7, 8);
    assert(hit == &box && "findMouseAt should return this when point is inside and no children");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_findMouseAt_returns_nullptr_when_miss) {
    NCURSES_SETUP;
    TBox box(20, 10, 5, 3, 1, "hello");
    TBox* hit = box.findMouseAt(0, 0);
    assert(hit == nullptr && "findMouseAt should return null when point is outside");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_findMouseAt_returns_deepest_child) {
    NCURSES_SETUP;
    TBox parent(40, 20, 0, 0, 1, "");
    TBox child(&parent, 15, 10, 2, 2, 1, "child");
    // Point inside both parent and child -> should return deepest (child)
    TBox* hit = parent.findMouseAt(5, 5);
    assert(hit == &child && "findMouseAt should return the deepest matching child");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - ITScrollable interface
// ============================================================================

TEST(test_TBox_getScrollValue_vertical) {
    NCURSES_SETUP;
    TBox box(10, 3, 1, vector<string>{"line1", "line2", "line3", "line4"});
    box.setScrollTop(1);
    assert(box.getScrollValue(ITScrollable::ORIENTATION_VERTICAL) == 1 && "getScrollValue(VERTICAL) should return scrollTop");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_getScrollMax_vertical) {
    NCURSES_SETUP;
    TBox box(10, 3, 1, vector<string>{"line1", "line2", "line3", "line4"});
    // bottom=4, height=3 -> max = 1
    assert(box.getScrollMax(ITScrollable::ORIENTATION_VERTICAL) == 1 && "getScrollMax(VERTICAL) should be correct");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setScrollValue_vertical_updates_scrollTop) {
    NCURSES_SETUP;
    TBox box(10, 3, 1, vector<string>{"line1", "line2", "line3", "line4"});
    box.setScrollValue(1, ITScrollable::ORIENTATION_VERTICAL);
    assert(box.getScrollTop() == 1 && "setScrollValue(VERTICAL) should update scrollTop");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_orientation_constants_are_correct) {
    assert(ITScrollable::ORIENTATION_VERTICAL == 0 && "VERTICAL should be 0");
    assert(ITScrollable::ORIENTATION_HORIZONTAL == 1 && "HORIZONTAL should be 1");
}

#endif
