#pragma once
#define NOMINMAX
#include "gesture_types.hpp"
#include <string>
#include <functional>
#ifdef _WIN32
#include <windows.h>
#endif

namespace gesture {

class LightController {
public:
    enum class Mode {
        SIMULATION,
        SERIAL,
        HTTP_WEBHOOK
    };

    struct Config {
        Mode        mode        = Mode::SIMULATION;
        std::string serial_port = "COM3";
        int         baud_rate   = 9600;
        std::string webhook_url = "";
    };

    LightController() = default;
    explicit LightController(const Config& config);
    ~LightController();

    bool       initialize();
    void       shutdown();
    void       process_gesture(const GestureResult& result);
    LightState state() const { return current_state_; }
    void       set_light(int index, bool on);
    void       set_all(bool on);

    using StateCallback = std::function<void(const LightState&)>;
    void on_state_change(StateCallback cb) { state_callback_ = std::move(cb); }

private:
    void apply_state(const LightState& new_state);
    void send_serial(const std::string& command);
    void send_http(const LightState& state);
    void print_state(const LightState& state) const;

    Config        config_;
    LightState    current_state_;
    StateCallback state_callback_;
#ifdef _WIN32
    HANDLE serial_handle_ = INVALID_HANDLE_VALUE;
#endif
};

} // namespace gesture