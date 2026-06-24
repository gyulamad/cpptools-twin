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

// ============================================================================
// TLineup Tests - Padding
// ============================================================================

TEST(test_TLineup_default_paddings_are_zero) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1);
    assert(lineup.getPaddingTop() == 0 && "Default paddingTop should be 0");
    assert(lineup.getPaddingLeft() == 0 && "Default paddingLeft should be 0");
    assert(lineup.getPaddingRight() == 0 && "Default paddingRight should be 0");
    assert(lineup.getPaddingBottom() == 0 && "Default paddingBottom should be 0");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_horizontal_paddingLeft_offsets_first_child) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 60, 15, 0, 0, 1, HORIZONTAL);
    lineup.setPaddingLeft(3);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getLeft() == 3 && "First child left should equal paddingLeft=3");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_horizontal_paddingTop_offsets_first_child) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 60, 15, 0, 0, 1, HORIZONTAL);
    lineup.setPaddingTop(2);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getTop() == 2 && "First child top should equal paddingTop=2");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_vertical_paddingLeft_offsets_first_child) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 60, 15, 0, 0, 1, VERTICAL);
    lineup.setPaddingLeft(3);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getLeft() == 3 && "First child left should equal paddingLeft=3 in vertical layout");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_vertical_paddingTop_offsets_first_child) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 60, 15, 0, 0, 1, VERTICAL);
    lineup.setPaddingTop(2);
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getTop() == 2 && "First child top should equal paddingTop=2 in vertical layout");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_horizontal_padding_with_gap) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 60, 15, 0, 0, 1, HORIZONTAL);
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

TEST(test_TLineup_setPaddings_batch) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 60, 15, 0, 0, 1, HORIZONTAL);
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

TEST(test_TLineup_relayout_after_padding_change) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 60, 15, 0, 0, 1, HORIZONTAL);
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

// ============================================================================
// TLineup Tests - fitChildren
// ============================================================================

TEST(test_TLineup_fitChildren_default_is_false) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    TLineup lineup(&parent, 1);
    assert(!lineup.getFitChildren() && "fitChildren should default to false");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_horizontal_fitChildren_stretches_height) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    // Lineup with fixed height=10.
    TLineup lineup(&parent, 60, 10, 0, 0, 1, HORIZONTAL);
    lineup.setFitChildren(true);
    TBox child(5, 3, 1, "hello");
    assert(child.getHeight() == 3 && "Child initial height is 3");
    lineup.addChild(&child);
    // With no padding: innerHeight = 10 - 0 - 0 = 10.
    assert(child.getHeight() == 10 && "Horizontal fitChildren stretches child height to inner space (10)");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_horizontal_fitChildren_with_padding) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    // Lineup with fixed height=12, paddingTop=2, paddingBottom=3.
    TLineup lineup(&parent, 60, 12, 0, 0, 1, HORIZONTAL);
    lineup.setPaddings(2, 0, 0, 3);
    lineup.setFitChildren(true);
    // innerHeight = 12 - 2 - 3 = 7.
    TBox child(5, 3, 1, "hello");
    lineup.addChild(&child);
    assert(child.getHeight() == 7 && "Child height stretched to inner space (12-2-3=7)");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_vertical_fitChildren_stretches_width) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    // Lineup with fixed width=50.
    TLineup lineup(&parent, 50, 30, 0, 0, 1, VERTICAL);
    lineup.setFitChildren(true);
    TBox child(8, 3, 1, "short");
    assert(child.getWidth() == 8 && "Child initial width is 8");
    lineup.addChild(&child);
    // With no padding: innerWidth = 50 - 0 - 0 = 50.
    assert(child.getWidth() == 50 && "Vertical fitChildren stretches child width to inner space (50)");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_vertical_fitChildren_with_padding) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    // Lineup with fixed width=60, paddingLeft=3, paddingRight=5.
    TLineup lineup(&parent, 60, 30, 0, 0, 1, VERTICAL);
    lineup.setPaddings(0, 3, 5, 0);
    lineup.setFitChildren(true);
    // innerWidth = 60 - 3 - 5 = 52.
    TBox child(8, 3, 1, "short");
    lineup.addChild(&child);
    assert(child.getWidth() == 52 && "Child width stretched to inner space (60-3-5=52)");
    NCURSES_TEARDOWN;
}

TEST(test_TLineup_fitChildren_multiple_children_all_stretched) {
    NCURSES_SETUP;
    TBox parent(80, 24, 0, 0, 1, "");
    // Horizontal lineup with height=10.
    TLineup lineup(&parent, 60, 10, 0, 0, 1, HORIZONTAL);
    lineup.setFitChildren(true);
    TBox child1(4, 2, 1, "ab");
    TBox child2(5, 3, 1, "hello");
    lineup.addChild(&child1);
    lineup.addChild(&child2);
    assert(child1.getHeight() == 10 && "First child stretched to inner height=10");
    assert(child2.getHeight() == 10 && "Second child also stretched to inner height=10");
    NCURSES_TEARDOWN;
}

#endif
