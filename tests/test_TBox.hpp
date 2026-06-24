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
// TBox Tests - applyWrap comprehensive tests
// ============================================================================

TEST(test_applyWrap_no_wrap_returns_raw_contents) {
    NCURSES_SETUP;
    TBox box(5, 10, 1, "this is a long line");
    // wrapText defaults to false
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 1 && "Should have exactly one line when wrapping is off");
    assert(wrapped[0] == "this is a long line" && "Content should match raw text");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_wraps_at_word_boundary) {
    NCURSES_SETUP;
    TBox box(10, 20, 1, "hello world foo bar");
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 3 && "Should wrap into three lines at width=10");
    assert(wrapped[0] == "hello" && "First line: 'hello'");
    assert(wrapped[1] == "world foo" && "Second line: 'world foo'");
    assert(wrapped[2] == "bar" && "Third line: 'bar'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_wraps_multiple_times) {
    NCURSES_SETUP;
    TBox box(8, 30, 1, "one two three four five six seven eight nine ten");
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped[0] == "one two" && "First line: 'one two' (fits in width=8)");
    assert(wrapped[1] == "three" && "Second line: 'three'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_hard_cut_when_no_space_in_width) {
    NCURSES_SETUP;
    TBox box(5, 10, 1, "supercalifragilistic");
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 4 && "Should hard-cut into four chunks of 5 chars each (20/5=4)");
    assert(wrapped[0] == "super" && "First chunk: 'super'");
    assert(wrapped[1] == "calif" && "Second chunk: 'calif'");
    assert(wrapped[2] == "ragil" && "Third chunk: 'ragil'");
    assert(wrapped[3] == "istic" && "Fourth chunk: 'istic'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_hard_cut_with_remainder) {
    NCURSES_SETUP;
    TBox box(5, 10, 1, "supercalifragilistihexp"); // 23 chars
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 5 && "Should hard-cut into five chunks (last one shorter)");
    assert(wrapped[0] == "super" && "First chunk: 'super'");
    assert(wrapped[1] == "calif" && "Second chunk: 'calif'");
    assert(wrapped[2] == "ragil" && "Third chunk: 'ragil'");
    assert(wrapped[3] == "istih" && "Fourth chunk: 'istih'");
    assert(wrapped[4] == "exp" && "Fifth chunk (remainder): 'exp'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_preserves_empty_lines) {
    NCURSES_SETUP;
    TBox box(10, 20, 1, vector<string>{"hello", "", "world"});
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 3 && "Should preserve three lines including empty one");
    assert(wrapped[0] == "hello" && "First line: 'hello'");
    assert(wrapped[1] == "" && "Second line should be empty");
    assert(wrapped[2] == "world" && "Third line: 'world'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_preserves_multiline_breaks) {
    NCURSES_SETUP;
    TBox box(10, 20, 1, vector<string>{"hello world", "foo bar baz"});
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    // First raw line wraps to: "hello" | "world"
    // Second raw line wraps to: "foo bar" | "baz"
    assert(wrapped.size() == 4 && "Should produce four wrapped lines from two raw lines");
    assert(wrapped[0] == "hello" && "Line 1: 'hello'");
    assert(wrapped[1] == "world" && "Line 2: 'world'");
    assert(wrapped[2] == "foo bar" && "Line 3: 'foo bar'");
    assert(wrapped[3] == "baz" && "Line 4: 'baz'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_line_exactly_equal_to_width) {
    NCURSES_SETUP;
    TBox box(10, 10, 1, "exactlyten"); // exactly 10 chars
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 1 && "Line equal to width should stay on one line");
    assert(wrapped[0] == "exactlyten" && "Content unchanged: 'exactlyten'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_line_shorter_than_width) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "short");
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 1 && "Short line should stay on one line");
    assert(wrapped[0] == "short" && "Content unchanged: 'short'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_skips_space_after_break) {
    NCURSES_SETUP;
    TBox box(10, 20, 1, "hello world foo bar baz qux");
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    // "hello" breaks at space -> next line starts with "world", not " world"
    assert(wrapped[0] == "hello" && "Line 1: 'hello' (space skipped)");
    assert(wrapped[1] == "world foo" && "Line 2: 'world foo' (no leading space)");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_word_exactly_at_boundary) {
    NCURSES_SETUP;
    TBox box(6, 10, 1, "abc def ghi jkl"); // each word is 3 chars + space = 6 per pair
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 4 && "Should produce four lines of 3-char words");
    assert(wrapped[0] == "abc" && "Line 1: 'abc'");
    assert(wrapped[1] == "def" && "Line 2: 'def'");
    assert(wrapped[2] == "ghi" && "Line 3: 'ghi'");
    assert(wrapped[3] == "jkl" && "Line 4: 'jkl'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_long_word_exceeds_width_no_space_within) {
    NCURSES_SETUP;
    // With forward-lookup fix: when no space within width, look ahead for next space.
    // "shortword" (9 chars) fits at first break point -> pos=10
    // "anotherlong" has no space in [10..19], but find(' ', 11) locates it at index 21
    //   so cut = 11, pushing the whole word "anotherlong", then skip space -> pos=22
    TBox box(10, 20, 1, "shortword anotherlong word");
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 3 && "Should wrap into three lines (whole words)");
    assert(wrapped[0] == "shortword" && "Line 1: 'shortword'");
    assert(wrapped[1] == "anotherlong" && "Line 2: whole word 'anotherlong' (forward-lookup avoids mid-word cut)");
    assert(wrapped[2] == "word" && "Line 3: 'word'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_multiple_consecutive_spaces) {
    NCURSES_SETUP;
    TBox box(10, 20, 1, "hello   world");
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 2 && "Should wrap into two lines");
    // Breaks at the last space within width, so line includes trailing spaces before skip
    assert(wrapped[0] == "hello  " && "Line 1: 'hello  ' (breaks at last space within width)");
    assert(wrapped[1] == "world" && "Line 2: 'world'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_resize_triggers_rewrap) {
    NCURSES_SETUP;
    TBox box(20, 10, 1, "hello world foo bar baz qux");
    box.setWrapText(true);
    vector<string> wrappedWide = box.getWrappedContents();
    assert(wrappedWide.size() == 2 && "At width=20 should wrap into two lines");

    // Shrink the box - setSize calls applyWrap
    box.setSize(8, 10);
    vector<string> wrappedNarrow = box.getWrappedContents();
    assert(wrappedNarrow.size() > wrappedWide.size() && "Narrower width should produce more wrapped lines");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_setContents_triggers_wrap) {
    NCURSES_SETUP;
    TBox box(10, 20, 1, "");
    box.setWrapText(true);
    // Initially empty
    assert(box.getWrappedContents().size() == 1 && "Empty content should produce one line");

    box.setContents("this is a longer text that wraps");
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() >= 2 && "New long content should be wrapped into multiple lines");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_empty_content) {
    NCURSES_SETUP;
    TBox box(10, 5, 1, vector<string>{});
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.empty() && "Empty content should produce zero lines");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_single_char_width) {
    NCURSES_SETUP;
    TBox box(1, 20, 1, "abcde");
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 5 && "Each character should be on its own line at width=1");
    assert(wrapped[0] == "a" && "Line 1: 'a'");
    assert(wrapped[1] == "b" && "Line 2: 'b'");
    assert(wrapped[2] == "c" && "Line 3: 'c'");
    assert(wrapped[3] == "d" && "Line 4: 'd'");
    assert(wrapped[4] == "e" && "Line 5: 'e'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_long_word_then_short_words) {
    NCURSES_SETUP;
    // With forward-lookup fix, the long word "superlongword" (13 chars) is kept intact.
    TBox box(10, 20, 1, "superlongword hi there ok bye now done end go set run play jump swim dive");
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped[0] == "superlongword" && "Line 1: whole word 'superlongword' (forward-lookup avoids mid-word cut)");
    assert(wrapped[1] == "hi there" && "Line 2: 'hi there'");
    NCURSES_TEARDOWN;
}

TEST(test_applyWrap_hard_cut_only_when_no_space_at_all) {
    NCURSES_SETUP;
    // When the entire remaining text has no space, hard-cut at width.
    TBox box(5, 10, 1, "supercalifragilistic");
    box.setWrapText(true);
    vector<string> wrapped = box.getWrappedContents();
    assert(wrapped.size() == 4 && "Should hard-cut into four chunks of 5 chars each (20/5=4)");
    assert(wrapped[0] == "super" && "First chunk: 'super'");
    assert(wrapped[1] == "calif" && "Second chunk: 'calif'");
    assert(wrapped[2] == "ragil" && "Third chunk: 'ragil'");
    assert(wrapped[3] == "istic" && "Fourth chunk: 'istic'");
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
    // parent.setLineup(HORIZONTAL);
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

// ============================================================================
// TBox Tests - onResize Propagation to Children
// ============================================================================

TEST(test_TBox_onResize_propagates_to_single_child) {
    NCURSES_SETUP;
    TBox parent(40, 20, 1, vector<string>{});
    TBox child(&parent, 30, 15, 0, 0, 2, "child");

    bool childResized = false;
    int resizeCols = 0, resizeRows = 0;

    child.onResizeSubscribe([&](int cols, int rows) -> TEventResult {
        childResized = true;
        resizeCols = cols;
        resizeRows = rows;
        return TEventResult::Handled;
    });

    parent.onResize(80, 40);

    assert(childResized && "Child should receive onResize event from parent");
    assert(resizeCols == 80 && "Child should receive correct cols value");
    assert(resizeRows == 40 && "Child should receive correct rows value");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_onResize_propagates_to_multiple_children) {
    NCURSES_SETUP;
    TBox parent(40, 20, 1, vector<string>{});
    TBox child1(&parent, 15, 10, 0, 0, 2, "child1");
    TBox child2(&parent, 15, 10, 0, 16, 3, "child2");
    TBox child3(&parent, 15, 10, 10, 0, 4, "child3");

    int callCount = 0;

    child1.onResizeSubscribe([&](int, int) -> TEventResult {
        callCount++;
        return TEventResult::Handled;
    });
    child2.onResizeSubscribe([&](int, int) -> TEventResult {
        callCount++;
        return TEventResult::Handled;
    });
    child3.onResizeSubscribe([&](int, int) -> TEventResult {
        callCount++;
        return TEventResult::Handled;
    });

    parent.onResize(100, 50);

    assert(callCount == 3 && "All three children should receive onResize event");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_onResize_propagates_to_nested_grandchildren) {
    NCURSES_SETUP;
    TBox grandparent(40, 20, 1, vector<string>{});
    TBox parent(&grandparent, 30, 15, 0, 0, 2, "parent");
    TBox child(&parent, 20, 10, 0, 0, 3, "child");

    bool grandChildResized = false;

    child.onResizeSubscribe([&](int cols, int rows) -> TEventResult {
        (void)cols; (void)rows;
        grandChildResized = true;
        return TEventResult::Handled;
    });

    grandparent.onResize(80, 40);

    assert(grandChildResized && "Grandchild should receive onResize event propagated through parent");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_onResize_stops_propagation_when_child_returns_Stop) {
    NCURSES_SETUP;
    TBox parent(40, 20, 1, vector<string>{});
    TBox child1(&parent, 15, 10, 0, 0, 2, "child1");
    TBox child2(&parent, 15, 10, 0, 16, 3, "child2");

    int callCount = 0;

    child1.onResizeSubscribe([&](int, int) -> TEventResult {
        callCount++;
        return TEventResult::Handled | TEventResult::Stop;
    });
    child2.onResizeSubscribe([&](int, int) -> TEventResult {
        callCount++;
        return TEventResult::None;
    });

    parent.onResize(80, 40);

    assert(callCount == 1 && "Propagation should stop after first child returns Stop");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_onResize_no_children_does_not_crash) {
    NCURSES_SETUP;
    TBox box(40, 20, 1, vector<string>{"solo"});

    // Should not crash when calling onResize with no children
    TEventResult result = box.onResize(80, 40);
    assert((result & TEventResult::Handled) == 0 && "No handlers set, so should return None");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - Construction and defaults
// ============================================================================

TEST(test_TBox_default_orientation_is_horizontal) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    assert(lineup.getOrientation() == HORIZONTAL && "Default orientation should be HORIZONTAL");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_explicit_vertical_orientation) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(VERTICAL);
    assert(lineup.getOrientation() == VERTICAL && "Orientation should be VERTICAL");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_default_gap_is_zero) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    assert(lineup.getGap() == 0 && "Default gap should be 0");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_autogrow_enabled_by_default) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(HORIZONTAL);
    assert(lineup.isAutoGrow() && "TBox should have autoGrow enabled");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_no_parent_constructor) {
    NCURSES_SETUP;
    TBox lineup(1);
    lineup.setLineup(HORIZONTAL);
    assert(lineup.getParent() == nullptr && "No-parent constructor should set parent to nullptr");
    assert(lineup.getOrientation() == HORIZONTAL && "Orientation should be HORIZONTAL");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_size_constructor_with_position) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 40, 10, 5, 3, 1);
    assert(lineup.getWidth() == 40 && "Width should be preserved from constructor");
    assert(lineup.getHeight() == 10 && "Height should be preserved from constructor");
    assert(lineup.getTop() == 5 && "Top should be 5");
    assert(lineup.getLeft() == 3 && "Left should be 3");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - Gap handling
// ============================================================================

TEST(test_TBox_setGap_updates_gap) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setGap(5);
    assert(lineup.getGap() == 5 && "Gap should be updated to 5");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - Horizontal child positioning
// ============================================================================

TEST(test_TBox_horizontal_first_child_at_origin) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(HORIZONTAL);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getTop() == 0 && "First child top should be 0 in horizontal layout");
    assert(child.getLeft() == 0 && "First child left should be 0 in horizontal layout");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_horizontal_second_child_placed_after_first) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(HORIZONTAL);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 3, 1, "world");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child2.getLeft() == 5 && "Second child left should be first width (5) when gap is 0");
    assert(child2.getTop() == 0 && "Second child top should align to min sibling top");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_horizontal_gap_between_children) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(HORIZONTAL);
    lineup.setGap(3);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 3, 1, "world");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child2.getLeft() == 8 && "Second child left should be first width + gap = 5+3=8");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_horizontal_multiple_children_cumulative_positions) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(HORIZONTAL);
    lineup.setGap(2);
    TBox child1(3, 2, 1, "abc");
    TBox child2(4, 2, 1, "defg");
    TBox child3(5, 2, 1, "hijkl");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    lineup.addChild(&child3);
    assert(child1.getLeft() == 0 && "First child at left=0");
    assert(child2.getLeft() == 5 && "Second child at left=3+2=5");
    assert(child3.getLeft() == 11 && "Third child at left=3+2+4+2=11");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_horizontal_aligns_to_min_top) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(HORIZONTAL);
    TBox child1(5, 3, 1, "hello");
    lineup.addChild(&child1);
    // Manually adjust first child top to simulate offset
    child1.setPosition(2, 0);
    TBox child2(4, 3, 1, "world");
    lineup.addChild(&child2);
    assert(child2.getTop() == 2 && "Second child should align to min sibling top=2");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - Vertical child positioning
// ============================================================================

TEST(test_TBox_vertical_first_child_at_origin) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(VERTICAL);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getTop() == 0 && "First child top should be 0 in vertical layout");
    assert(child.getLeft() == 0 && "First child left should be 0 in vertical layout");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_vertical_second_child_placed_below_first) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(VERTICAL);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 4, 1, "world\n!");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child2.getTop() == 3 && "Second child top should be first height (3) when gap is 0");
    assert(child2.getLeft() == 0 && "Second child left should align to min sibling left");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_vertical_gap_between_children) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(VERTICAL);
    lineup.setGap(3);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 4, 1, "world\n!");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child2.getTop() == 6 && "Second child top should be first height + gap = 3+3=6");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_vertical_multiple_children_cumulative_positions) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(VERTICAL);
    lineup.setGap(2);
    TBox child1(3, 2, 1, "ab");
    TBox child2(4, 3, 1, "def\ngh");
    TBox child3(5, 4, 1, "hij\nklm\nnop");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    lineup.addChild(&child3);
    assert(child1.getTop() == 0 && "First child at top=0");
    assert(child2.getTop() == 4 && "Second child at top=2+2=4");
    assert(child3.getTop() == 9 && "Third child at top=2+2+3+2=9");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_vertical_aligns_to_min_left) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(VERTICAL);
    TBox child1(5, 3, 1, "hello");
    lineup.addChild(&child1);
    // Manually adjust first child left to simulate offset
    child1.setPosition(0, 2);
    TBox child2(4, 3, 1, "world");
    lineup.addChild(&child2);
    assert(child2.getLeft() == 2 && "Second child should align to min sibling left=2");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - relayout
// ============================================================================

TEST(test_TBox_relayout_after_gap_change_horizontal) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(HORIZONTAL);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 3, 1, "world");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child2.getLeft() == 5 && "Initial left should be 5 with gap=0");
    // Change gap and verify relayout through setGap
    lineup.setGap(4);
    assert(child2.getLeft() == 9 && "After gap=4, second child left should be 5+4=9");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_relayout_after_gap_change_vertical) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(VERTICAL);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 4, 1, "world\n!");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child2.getTop() == 3 && "Initial top should be 3 with gap=0");
    // Change gap and verify relayout through setGap
    lineup.setGap(4);
    assert(child2.getTop() == 7 && "After gap=4, second child top should be 3+4=7");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_relayout_updates_bounds) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    lineup.setLineup(HORIZONTAL);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 3, 1, "world");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    int rightBefore = lineup.getRight();
    assert(rightBefore == 9 && "Right should be 5+4=9 with gap=0");
    lineup.setGap(6);
    // After relayout: child1 at left=0 (width=5), child2 at left=11 (5+6) width=4 => right=15
    assert(lineup.getRight() == 15 && "Right should update after gap increase");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TBox Tests - Padding
// ============================================================================

TEST(test_TBox_default_paddings_are_zero) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 1);
    assert(lineup.getPaddingTop() == 0 && "Default paddingTop should be 0");
    assert(lineup.getPaddingLeft() == 0 && "Default paddingLeft should be 0");
    assert(lineup.getPaddingRight() == 0 && "Default paddingRight should be 0");
    assert(lineup.getPaddingBottom() == 0 && "Default paddingBottom should be 0");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_horizontal_paddingLeft_offsets_first_child) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 60, 15, 0, 0, 1);
    lineup.setLineup(HORIZONTAL);
    lineup.setPaddingLeft(3);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getLeft() == 3 && "First child left should equal paddingLeft=3");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_horizontal_paddingTop_offsets_first_child) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 60, 15, 0, 0, 1);
    lineup.setLineup(HORIZONTAL);
    lineup.setPaddingTop(2);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getTop() == 2 && "First child top should equal paddingTop=2");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_vertical_paddingLeft_offsets_first_child) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 60, 15, 0, 0, 1);
    lineup.setLineup(VERTICAL);
    lineup.setPaddingLeft(3);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getLeft() == 3 && "First child left should equal paddingLeft=3 in vertical layout");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_vertical_paddingTop_offsets_first_child) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 60, 15, 0, 0, 1);
    lineup.setLineup(VERTICAL);
    lineup.setPaddingTop(2);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getTop() == 2 && "First child top should equal paddingTop=2 in vertical layout");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_horizontal_padding_with_gap) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 60, 15, 0, 0, 1);
    lineup.setLineup(HORIZONTAL);
    lineup.setPaddingLeft(3);
    lineup.setGap(2);
    TBox child1(4, 3, 1, "abc");
    TBox child2(5, 3, 1, "hello");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child1.getLeft() == 3 && "First at paddingLeft=3");
    assert(child2.getLeft() == 9 && "Second at paddingLeft+firstWidth+gap = 3+4+2=9");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_setPaddings_batch) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 60, 15, 0, 0, 1);
    lineup.setLineup(HORIZONTAL);
    lineup.setPaddings(1, 3, 2, 1);
    assert(lineup.getPaddingTop() == 1 && "paddingTop should be 1");
    assert(lineup.getPaddingLeft() == 3 && "paddingLeft should be 3");
    assert(lineup.getPaddingRight() == 2 && "paddingRight should be 2");
    assert(lineup.getPaddingBottom() == 1 && "paddingBottom should be 1");
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getLeft() == 3 && "Child left offset by paddingLeft=3");
    assert(child.getTop() == 1 && "Child top offset by paddingTop=1");
    NCURSES_TEARDOWN;
}

TEST(test_TBox_relayout_after_padding_change) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TBox lineup(&parent, 60, 15, 0, 0, 1);
    lineup.setLineup(HORIZONTAL);
    TBox child1(4, 3, 1, "abc");
    TBox child2(5, 3, 1, "hello");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child1.getLeft() == 0 && "Initial left=0 with no padding");
    // Change paddingLeft and verify relayout.
    lineup.setPaddingLeft(4);
    assert(child1.getLeft() == 4 && "After setPaddingLeft, first child moves to 4");
    assert(child2.getLeft() == 8 && "Second at paddingLeft+firstWidth = 4+4=8 (gap=0)");
    NCURSES_TEARDOWN;
}

#endif
