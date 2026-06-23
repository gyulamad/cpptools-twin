#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
#include "../TColorPicker.hpp"
#include "test_NcursesScreen.hpp"

// ============================================================================
// TColorChannel Tests
// ============================================================================

TEST(test_TColorChannel_default_value_is_zero) {
    NCURSES_SETUP;
    TColorChannel ch;
    assert(ch.getValue() == 0 && "Default value should be 0");
    NCURSES_TEARDOWN;
}

TEST(test_TColorChannel_setValue_sets_correct_value) {
    NCURSES_SETUP;
    TColorChannel ch;
    ch.setValue(128);
    assert(ch.getValue() == 128 && "Value should be 128");
    NCURSES_TEARDOWN;
}

TEST(test_TColorChannel_setValue_clamps_negative_to_zero) {
    NCURSES_SETUP;
    TColorChannel ch;
    ch.setValue(-50);
    assert(ch.getValue() == 0 && "Negative value should clamp to 0");
    NCURSES_TEARDOWN;
}

TEST(test_TColorChannel_setValue_clamps_above_255_to_255) {
    NCURSES_SETUP;
    TColorChannel ch;
    ch.setValue(300);
    assert(ch.getValue() == 255 && "Value above 255 should clamp to 255");
    NCURSES_TEARDOWN;
}

TEST(test_TColorChannel_constructor_with_initial_value) {
    NCURSES_SETUP;
    TColorChannel ch(200);
    assert(ch.getValue() == 200 && "Constructor value should be 200");
    NCURSES_TEARDOWN;
}

TEST(test_TColorChannel_getScrollValue_returns_value) {
    NCURSES_SETUP;
    TColorChannel ch(75);
    assert(ch.getScrollValue(0) == 75 && "getScrollValue should return the channel value");
    NCURSES_TEARDOWN;
}

TEST(test_TColorChannel_getScrollMin_returns_zero) {
    NCURSES_SETUP;
    TColorChannel ch;
    assert(ch.getScrollMin(0) == 0 && "getScrollMin should always be 0");
    NCURSES_TEARDOWN;
}

TEST(test_TColorChannel_getScrollMax_returns_255) {
    NCURSES_SETUP;
    TColorChannel ch;
    assert(ch.getScrollMax(0) == 255 && "getScrollMax should always be 255");
    NCURSES_TEARDOWN;
}

TEST(test_TColorChannel_setScrollValue_delegates_to_setValue) {
    NCURSES_SETUP;
    TColorChannel ch;
    ch.setScrollValue(180, 0);
    assert(ch.getValue() == 180 && "setScrollValue should set the value via setValue");
    NCURSES_TEARDOWN;
}

TEST(test_TColorChannel_notify_scroll_change_on_setValue) {
    NCURSES_SETUP;
    TColorChannel ch;
    bool notified = false;
    ch.onScrollChangeSubscribe([&]() { notified = true; });
    ch.setValue(50);
    assert(notified && "setValue should notify scroll change subscribers");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TColorPicker Tests - Construction and layout
// ============================================================================

TEST(test_TColorPicker_sets_initial_channel_values) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    TColorPicker picker(&root, 50, 6, 0, 0,
                        1, 2, 3,
                        4, 5,
                        palette, 100, 150, 200);
    assert(picker.getChannelR().getValue() == 100 && "Red channel should be 100");
    assert(picker.getChannelG().getValue() == 150 && "Green channel should be 150");
    assert(picker.getChannelB().getValue() == 200 && "Blue channel should be 200");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPicker_default_initial_values_are_zero) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    TColorPicker picker(&root, 50, 6, 0, 0,
                        1, 2, 3,
                        4, 5,
                        palette);
    assert(picker.getChannelR().getValue() == 0 && "Default red should be 0");
    assert(picker.getChannelG().getValue() == 0 && "Default green should be 0");
    assert(picker.getChannelB().getValue() == 0 && "Default blue should be 0");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPicker_minimum_height_is_four) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    TColorPicker picker(&root, 50, 2, 0, 0,
                        1, 2, 3,
                        4, 5,
                        palette);
    assert(picker.getHeight() == 4 && "Height should be clamped to minimum 4");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPicker_height_above_minimum_is_preserved) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    TColorPicker picker(&root, 50, 8, 0, 0,
                        1, 2, 3,
                        4, 5,
                        palette);
    assert(picker.getHeight() == 8 && "Height should be preserved when above minimum");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPicker_width_too_small_throws_error) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    bool threw = false;
    try {
        // Layout: label(2) + gap(1) + scrollbar(?) + gap(2) + value(4) = width
        // Minimum sbWidth is 1, so minimum width = 2+1+1+2+4 = 10
        TColorPicker picker(&root, 5, 6, 0, 0,
                            1, 2, 3,
                            4, 5,
                            palette);
    } catch (const exception& e) {
        assert(str_contains(e.what(), "width too small") && "Exception should mention width too small");
        threw = true;
    }
    assert(threw && "TColorPicker should throw when width is too small");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPicker_positions_are_set_correctly) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    TColorPicker picker(&root, 50, 6, 3, 5,
                        1, 2, 3,
                        4, 5,
                        palette);
    assert(picker.getTop() == 3 && "Top should be 3");
    assert(picker.getLeft() == 5 && "Left should be 5");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPicker_width_is_preserved) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    TColorPicker picker(&root, 50, 6, 0, 0,
                        1, 2, 3,
                        4, 5,
                        palette);
    assert(picker.getWidth() == 50 && "Width should be preserved");
    NCURSES_TEARDOWN;
}

// ============================================================================
// TColorPicker Tests - Channel interaction and preview
// ============================================================================

TEST(test_TColorPicker_channel_change_updates_value) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    TColorPicker picker(&root, 50, 6, 0, 0,
                        1, 2, 3,
                        4, 5,
                        palette);
    picker.getChannelR().setValue(255);
    assert(picker.getChannelR().getValue() == 255 && "Red should update to 255");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPicker_channel_change_triggers_preview_update) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    bool previewUpdated = false;
    TColorPicker picker(&root, 50, 6, 0, 0,
                        1, 2, 3,
                        4, 5,
                        palette);
    // Subscribe to the channel's scroll change to verify callback chain works.
    picker.getChannelR().onScrollChangeSubscribe([&]() { previewUpdated = true; });
    picker.getChannelR().setValue(128);
    assert(previewUpdated && "Channel setValue should trigger subscribed callbacks");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPicker_all_channels_independently_updatable) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    TColorPicker picker(&root, 50, 6, 0, 0,
                        1, 2, 3,
                        4, 5,
                        palette);
    picker.getChannelR().setValue(10);
    picker.getChannelG().setValue(20);
    picker.getChannelB().setValue(30);
    assert(picker.getChannelR().getValue() == 10 && "Red should be independent");
    assert(picker.getChannelG().getValue() == 20 && "Green should be independent");
    assert(picker.getChannelB().getValue() == 30 && "Blue should be independent");
    NCURSES_TEARDOWN;
}

TEST(test_TColorPicker_channels_clamp_values_in_picker) {
    NCURSES_SETUP;
    TBox root(80, 24, 0, 0, 1, "");
    TColorPairPalette palette;
    TColorPicker picker(&root, 50, 6, 0, 0,
                        1, 2, 3,
                        4, 5,
                        palette);
    picker.getChannelR().setValue(-100);
    picker.getChannelG().setValue(500);
    assert(picker.getChannelR().getValue() == 0 && "Red should clamp to 0");
    assert(picker.getChannelG().getValue() == 255 && "Green should clamp to 255");
    NCURSES_TEARDOWN;
}

#endif // TEST
