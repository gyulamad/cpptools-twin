#pragma once

#ifdef TEST

#include <string.h>
#include "../../misc/TEST.hpp"
#include "../TLineup.hpp"
#include "test_NcursesScreen.hpp"

// ============================================================================
// TLineup Tests - Construction and defaults
// ============================================================================

TEST(test_TLineup_default_orientation_is_horizontal) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1);
    assert(lineup.getOrientation() == HORIZONTAL && "Default orientation should be HORIZONTAL");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_explicit_vertical_orientation) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, VERTICAL);
    assert(lineup.getOrientation() == VERTICAL && "Orientation should be VERTICAL");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_default_gap_is_zero) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1);
    assert(lineup.getGap() == 0 && "Default gap should be 0");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_autogrow_enabled_by_default) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1);
    assert(lineup.isAutoGrow() && "TLineup should have autoGrow enabled");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_no_parent_constructor) {
    NCURSES_SETUP;
    TLineup lineup(1, HORIZONTAL);
    assert(lineup.getParent() == nullptr && "No-parent constructor should set parent to nullptr");
    assert(lineup.getOrientation() == HORIZONTAL && "Orientation should be HORIZONTAL");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_size_constructor_with_position) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 40, 10, 5, 3, 1, VERTICAL);
    assert(lineup.getWidth() == 40 && "Width should be preserved from constructor");
    assert(lineup.getHeight() == 10 && "Height should be preserved from constructor");
    assert(lineup.getTop() == 5 && "Top should be 5");
    assert(lineup.getLeft() == 3 && "Left should be 3");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TLineup Tests - Gap handling
// ============================================================================

TEST(test_TLineup_setGap_updates_gap) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1);
    lineup.setGap(5);
    assert(lineup.getGap() == 5 && "Gap should be updated to 5");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TLineup Tests - Horizontal child positioning
// ============================================================================

TEST(test_TLineup_horizontal_first_child_at_origin) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, HORIZONTAL);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getTop() == 0 && "First child top should be 0 in horizontal layout");
    assert(child.getLeft() == 0 && "First child left should be 0 in horizontal layout");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_horizontal_second_child_placed_after_first) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, HORIZONTAL);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 3, 1, "world");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child2.getLeft() == 5 && "Second child left should be first width (5) when gap is 0");
    assert(child2.getTop() == 0 && "Second child top should align to min sibling top");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_horizontal_gap_between_children) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, HORIZONTAL);
    lineup.setGap(3);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 3, 1, "world");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child2.getLeft() == 8 && "Second child left should be first width + gap = 5+3=8");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_horizontal_multiple_children_cumulative_positions) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, HORIZONTAL);
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

TEST(test_TLineup_horizontal_aligns_to_min_top) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, HORIZONTAL);
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
// TLineup Tests - Vertical child positioning
// ============================================================================

TEST(test_TLineup_vertical_first_child_at_origin) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, VERTICAL);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getTop() == 0 && "First child top should be 0 in vertical layout");
    assert(child.getLeft() == 0 && "First child left should be 0 in vertical layout");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_vertical_second_child_placed_below_first) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, VERTICAL);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 4, 1, "world\n!");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child2.getTop() == 3 && "Second child top should be first height (3) when gap is 0");
    assert(child2.getLeft() == 0 && "Second child left should align to min sibling left");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_vertical_gap_between_children) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, VERTICAL);
    lineup.setGap(3);
    TBox child1(5, 3, 1, "hello");
    TBox child2(4, 4, 1, "world\n!");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child2.getTop() == 6 && "Second child top should be first height + gap = 3+3=6");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_vertical_multiple_children_cumulative_positions) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, VERTICAL);
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

TEST(test_TLineup_vertical_aligns_to_min_left) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, VERTICAL);
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
// TLineup Tests - relayout
// ============================================================================

TEST(test_TLineup_relayout_after_gap_change_horizontal) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, HORIZONTAL);
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

TEST(test_TLineup_relayout_after_gap_change_vertical) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, VERTICAL);
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

TEST(test_TLineup_relayout_updates_bounds) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1, HORIZONTAL);
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

#endif
