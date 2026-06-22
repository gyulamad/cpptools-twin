#pragma once

#ifdef TEST

#include "../../misc/TEST.hpp"
#include "../TColorPair.hpp"

// Helper to compare TColorPair — uses explicit reference binding to avoid C++20 ambiguity.
static bool colorPairsEqual(const TColorPair& _a, const TColorPair& _b) {
    TColorPair a = _a;
    return (a == _b);
}

TEST(test_TColorPair_constructor_and_equality) {
    TColorPair a(1, 2);
    TColorPair b(1, 2);
    assert(colorPairsEqual(a, b) && "Same values should be equal");
}

TEST(test_TColorPair_equality_same_values) {
    TColorPair a(1, 0);
    TColorPair b(1, 0);
    assert(colorPairsEqual(a, b) && "Equal color pairs should compare equal");
}

TEST(test_TColorPair_equality_different_fg) {
    TColorPair a(1, 0);
    TColorPair b(2, 0);
    bool neq = !colorPairsEqual(a, b);
    assert(neq && "Different foreground colors should not be equal");
}

TEST(test_TColorPair_equality_different_bg) {
    TColorPair a(1, 0);
    TColorPair b(1, 7);
    bool neq = !colorPairsEqual(a, b);
    assert(neq && "Different background colors should not be equal");
}

#endif