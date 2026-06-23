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

// ============================================================================
// TBox Tests - Auto Grow / Shrink (content)
// ============================================================================

TEST(test_TBox_autoGrow_grows_when_content_expands) {
    NCURSES_SETUP;
    TBox box(10, 3, 1, vector<string>{"line1", "line2"});
    // bottom=2, height grows to 2 initially via constructor auto-grow? No — default is false.
    assert(!box.isAutoGrow() && "autoGrow should be off by default");
    box.setAutoGrow(true);
    assert(box.getHeight() == 2 && "Height should match content lines after enabling autoGrow");
    // Now add more content
    box.setContents(vector<string>{"line1", "line2", "line3", "line4"});
    assert(box.getHeight() == 4 && "Height should grow to 4 when content expands");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_autoGrow_shrinks_when_content_removed) {
    NCURSES_SETUP;
    TBox box(10, 5, 1, vector<string>{"line1", "line2", "line3", "line4", "line5"});
    // Force a fixed size first so we can see auto-grow take over.
    assert(box.getWidth() == 10 && "Initial width should be 10");
    box.setAutoGrow(true);
    // After enabling, height = bottom = 5 (content lines)
    int heightAfterEnable = box.getHeight();
    assert(heightAfterEnable == 5 && "Height should match content after autoGrow enabled");
    // Remove some content — should shrink
    box.setContents(vector<string>{"line1"});
    assert(box.getHeight() == 1 && "Height should shrink to 1 when content reduced");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_autoGrow_parent_shrinks_when_child_setSize_smaller) {
    NCURSES_SETUP;
    TBox parent(40, 20, 1, vector<string>{});
    // Constructor: (parent, width, height, top, left, colorPair, contents)
    TBox child(&parent, 30, 15, 0, 0, 2, "child");
    // Child at top=0, height=15 -> parent bottom = max(content=0, 0+15) = 15
    assert(parent.getBottom() == 15 && "Parent bottom should be 15 from child extent");

    parent.setAutoGrow(true);
    int hAfterEnable = parent.getHeight();
    (void)hAfterEnable; // height may have been adjusted by auto-grow

    // Now shrink the child — parent with autoGrow should cascade-shrink
    child.setSize(30, 5);
    assert(parent.getBottom() == 5 && "Parent bottom should update to 5 after child shrinks");
    int hAfter = parent.getHeight();
    assert(hAfter <= hAfterEnable && "Parent height should shrink or stay same when child shrinks");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_autoGrow_parent_shrinks_when_child_moved_closer_to_origin) {
    NCURSES_SETUP;
    TBox parent(40, 20, 1, vector<string>{});
    // Constructor: (parent, width, height, top, left, colorPair, contents)
    TBox child(&parent, 30, 15, 5, 0, 2, "child");
    // Child at top=5, height=15 -> parent bottom = max(content=0, 5+15) = 20
    assert(parent.getBottom() == 20 && "Parent bottom should be 20 from child extent");

    parent.setAutoGrow(true);
    int hAfterEnable = parent.getHeight();

    // Move child closer to top — parent with autoGrow should cascade-shrink
    child.setPosition(0, 0);
    assert(parent.getBottom() == 15 && "Parent bottom should update to 15 after child moves up");
    int hAfter = parent.getHeight();
    assert(hAfter <= hAfterEnable && "Parent height should shrink or stay same when child moves closer");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_autoGrow_cascades_through_grandparent) {
    NCURSES_SETUP;
    TBox grandparent(60, 30, 1, vector<string>{});
    // Constructor: (parent, width, height, top, left, colorPair, contents)
    TBox parent(&grandparent, 40, 20, 0, 0, 2, vector<string>{});
    TBox child(&parent, 30, 15, 0, 0, 3, "child");

    grandparent.setAutoGrow(true);
    parent.setAutoGrow(true);

    // Initial: child bottom=15 (from content), parent bottom=15, grandparent bottom=20 (via children)
    int gpH = grandparent.getHeight();

    // Shrink the child — should cascade up through both ancestors
    child.setSize(30, 3);
    assert(parent.getBottom() == 3 && "Parent bottom should update to 3");
    int parentHAfter = parent.getHeight();
    (void)parentHAfter;

    // Grandparent should also have shrunk or stayed same
    int gpHAfter = grandparent.getHeight();
    assert(gpHAfter <= gpH && "Grandparent height should shrink or stay same when child shrinks");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_autoGrow_does_not_shrink_when_disabled) {
    NCURSES_SETUP;
    TBox parent(40, 20, 1, vector<string>{});
    // Constructor: (parent, width, height, top, left, colorPair, contents)
    TBox child(&parent, 30, 15, 0, 0, 2, "child");

    // Parent autoGrow is OFF — should NOT shrink when child shrinks
    assert(!parent.isAutoGrow() && "autoGrow should be off by default");
    int hBefore = parent.getHeight();
    assert(hBefore == 20 && "Parent height should remain at fixed 20");

    child.setSize(30, 5);
    assert(parent.getHeight() == 20 && "Parent height stays at 20 when autoGrow is disabled and child shrinks");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_autoGrow_grows_when_child_setSize_larger) {
    NCURSES_SETUP;
    TBox parent(40, 10, 1, vector<string>{});
    // Constructor: (parent, width, height, top, left, colorPair, contents)
    TBox child(&parent, 30, 5, 0, 0, 2, "child");

    assert(parent.getBottom() == 5 && "Parent bottom should be 5 from small child");
    parent.setAutoGrow(true);
    int hAfterEnable = parent.getHeight();

    // Now enlarge the child — parent with autoGrow should grow
    child.setSize(30, 18);
    assert(parent.getBottom() == 18 && "Parent bottom should update to 18 after child grows");
    int hAfter = parent.getHeight();
    assert(hAfter >= hAfterEnable && "Parent height should grow when child enlarges");
    NCURSES_TEARDOWN;
}

#endif
