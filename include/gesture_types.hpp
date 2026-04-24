#pragma once

#include <string>
#include <array>
#include <cstdint>

namespace gesture {

// ============================================================
// Gesture enumeration — each gesture maps to a light action
// ============================================================
enum class GestureType : uint8_t {
    NONE         = 0,   // No hand detected / unknown
    FIST         = 1,   // ✊ All lights OFF
    OPEN_PALM    = 2,   // ✋ All lights ON
    ONE_FINGER   = 3,   // ☝️ Light 1
    TWO_FINGERS  = 4,   // ✌️ Light 2
    THREE_FINGERS= 5,   // 🤟 Light 3

    COUNT        = 6    // Total number of gesture classes
};

// ============================================================
// Human-readable gesture names
// ============================================================
inline constexpr std::array<const char*, static_cast<size_t>(GestureType::COUNT)> GESTURE_NAMES = {
    "None",
    "Fist",
    "Open Palm",
    "One Finger",
    "Two Fingers",
    "Three Fingers"
};

inline const char* gesture_name(GestureType g) {
    auto idx = static_cast<size_t>(g);
    return (idx < GESTURE_NAMES.size()) ? GESTURE_NAMES[idx] : "Unknown";
}

// ============================================================
// Classification result
// ============================================================
struct GestureResult {
    GestureType gesture    = GestureType::NONE;
    float       confidence = 0.0f;
    
    bool is_valid(float threshold = 0.7f) const {
        return gesture != GestureType::NONE && confidence >= threshold;
    }
};

// ============================================================
// Light state
// ============================================================
struct LightState {
    static constexpr int NUM_LIGHTS = 3;
    std::array<bool, NUM_LIGHTS> lights = {false, false, false};
    
    void all_on()  { lights.fill(true); }
    void all_off() { lights.fill(false); }
    
    void set(int index, bool state) {
        if (index >= 0 && index < NUM_LIGHTS)
            lights[index] = state;
    }
    
    bool get(int index) const {
        return (index >= 0 && index < NUM_LIGHTS) ? lights[index] : false;
    }
    
    bool operator==(const LightState& other) const {
        return lights == other.lights;
    }
    
    bool operator!=(const LightState& other) const {
        return !(*this == other);
    }
};

// ============================================================
// Configuration
// ============================================================
struct AppConfig {
    std::string model_path   = "model/gesture_model.pt";
    int         camera_id    = 0;
    int         frame_width  = 640;
    int         frame_height = 480;
    float       confidence_threshold = 0.7f;
    int         smoothing_window     = 5;     // frames to average
    bool        debug_mode           = false;
    bool        mirror               = true;  // flip horizontal
    std::string serial_port          = "";    // e.g., "COM3" for Arduino
    int         serial_baud          = 9600;
};

} // namespace gesture