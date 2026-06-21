#pragma once

#include <cstdint>

// -------------------------------------------------------
// TEventResult — bitmask for event propagation control
// -------------------------------------------------------
// Two independent bits: did the handler consume it, and should
// propagation stop?  This removes all ambiguity from a plain bool.
//
//   Handled bit  (1<<0) — this handler consumed/processed the event
//   Stop bit     (1<<1) — no further handlers should see this event

enum class TEventResult : uint8_t {
    None      = 0,       // unhandled + continue propagation
    Handled   = 1 << 0,  // handled but let others see it too
    Stop      = 1 << 1,  // force-stop (emergency / not-handled-but-abort)
};

inline TEventResult operator|(TEventResult a, TEventResult b) { 
    return static_cast<TEventResult>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); 
}

inline TEventResult& operator|=(TEventResult& a, TEventResult b) { 
    a = a | b; 
    return a; 
}

inline uint8_t operator&(TEventResult a, TEventResult b) { 
    return static_cast<uint8_t>(a) & static_cast<uint8_t>(b); 
}
