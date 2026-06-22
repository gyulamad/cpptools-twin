#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
#include "../TEventResult.hpp"

// ============================================================================
// TEventResult Tests - Bitmask operators
// ============================================================================

TEST(test_TEventResult_none_is_zero) {
    assert(static_cast<uint8_t>(TEventResult::None) == 0 && "None should be zero");
}

TEST(test_TEventResult_handled_bit_set) {
    uint8_t val = static_cast<uint8_t>(TEventResult::Handled);
    assert((val & (1 << 0)) != 0 && "Handled should have bit 0 set");
}

TEST(test_TEventResult_stop_bit_set) {
    uint8_t val = static_cast<uint8_t>(TEventResult::Stop);
    assert((val & (1 << 1)) != 0 && "Stop should have bit 1 set");
}

TEST(test_TEventResult_or_combines_handled_and_stop) {
    TEventResult combined = TEventResult::Handled | TEventResult::Stop;
    assert((combined & TEventResult::Handled) && "Combined should include Handled");
    assert((combined & TEventResult::Stop) && "Combined should include Stop");
}

TEST(test_TEventResult_or_assign_accumulates) {
    TEventResult result = TEventResult::None;
    result |= TEventResult::Handled;
    assert((result & TEventResult::Handled) && "Should have Handled after |=");
    result |= TEventResult::Stop;
    assert((result & TEventResult::Stop) && "Should also have Stop");
}

TEST(test_TEventResult_and_checks_bit) {
    TEventResult combined = TEventResult::Handled | TEventResult::Stop;
    uint8_t hasHandled = combined & TEventResult::Handled;
    assert(hasHandled != 0 && "And operator should detect Handled bit");
}

TEST(test_TEventResult_none_and_handled_is_zero) {
    TEventResult none = TEventResult::None;
    uint8_t result = none & TEventResult::Handled;
    assert(result == 0 && "None AND Handled should be zero");
}

#endif
