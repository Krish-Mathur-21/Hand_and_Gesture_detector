#define NOMINMAX
#include "light_controller.hpp"
#include "logger.hpp"
#include <sstream>
#include <iomanip>

namespace gesture {

LightController::LightController(const Config& config) : config_(config) {}
LightController::~LightController() { shutdown(); }

bool LightController::initialize() {
    switch (config_.mode) {
        case Mode::SIMULATION:
            logger::info("Light controller: SIMULATION mode");
            return true;
        case Mode::SERIAL:
#ifdef _WIN32
        {
            logger::info("Opening serial port {}...", config_.serial_port);
            serial_handle_ = CreateFileA(config_.serial_port.c_str(),
                GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (serial_handle_ == INVALID_HANDLE_VALUE) {
                logger::error("Failed to open serial port {}", config_.serial_port);
                logger::info("Falling back to SIMULATION mode");
                config_.mode = Mode::SIMULATION;
                return true;
            }
            DCB dcb{}; dcb.DCBlength = sizeof(DCB);
            GetCommState(serial_handle_, &dcb);
            dcb.BaudRate = config_.baud_rate;
            dcb.ByteSize = 8; dcb.StopBits = ONESTOPBIT; dcb.Parity = NOPARITY;
            SetCommState(serial_handle_, &dcb);
            COMMTIMEOUTS to{};
            to.ReadIntervalTimeout = to.ReadTotalTimeoutConstant =
            to.WriteTotalTimeoutConstant = 50;
            SetCommTimeouts(serial_handle_, &to);
            logger::info("Serial port {} opened at {} baud",
                         config_.serial_port, config_.baud_rate);
            return true;
        }
#else
            logger::error("Serial mode not implemented for this platform");
            return false;
#endif
        case Mode::HTTP_WEBHOOK:
            logger::info("Light controller: HTTP mode -> {}", config_.webhook_url);
            return true;
        default: return false;
    }
}

void LightController::shutdown() {
#ifdef _WIN32
    if (serial_handle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(serial_handle_);
        serial_handle_ = INVALID_HANDLE_VALUE;
        logger::info("Serial port closed.");
    }
#endif
}

void LightController::process_gesture(const GestureResult& result) {
    if (!result.is_valid()) return;
    LightState s = current_state_;
    switch (result.gesture) {
case GestureType::FIST:          s.all_off(); break;
case GestureType::OPEN_PALM:     s.all_on();  break;
case GestureType::ONE_FINGER:    s.all_off(); s.set(0, true); break;
case GestureType::TWO_FINGERS:   s.all_off(); s.set(1, true); break;
case GestureType::THREE_FINGERS: s.all_off(); s.set(2, true); break;
        default: return;
    }
    if (s != current_state_) apply_state(s);
}

void LightController::set_light(int index, bool on) {
    LightState s = current_state_; s.set(index, on);
    if (s != current_state_) apply_state(s);
}

void LightController::set_all(bool on) {
    LightState s; if (on) s.all_on(); else s.all_off();
    if (s != current_state_) apply_state(s);
}

void LightController::apply_state(const LightState& ns) {
    current_state_ = ns;
    switch (config_.mode) {
        case Mode::SIMULATION:   print_state(current_state_); break;
        case Mode::SERIAL: {
            std::ostringstream cmd;
            cmd << "L";
            for (int i = 0; i < LightState::NUM_LIGHTS; ++i) {
                if (i > 0) cmd << ",";
                cmd << (current_state_.get(i) ? 1 : 0);
            }
            cmd << "\n";
            send_serial(cmd.str());
            break;
        }
        case Mode::HTTP_WEBHOOK: send_http(current_state_); break;
    }
    if (state_callback_) state_callback_(current_state_);
}

void LightController::send_serial(const std::string& command) {
#ifdef _WIN32
    if (serial_handle_ == INVALID_HANDLE_VALUE) return;
    DWORD bw;
    WriteFile(serial_handle_, command.c_str(),
              static_cast<DWORD>(command.size()), &bw, nullptr);
    logger::info("Serial TX: {}", command);
#endif
}

void LightController::send_http(const LightState& state) {
    logger::info("HTTP POST -> {} lights=[{},{},{}]",
                 config_.webhook_url,
                 state.get(0), state.get(1), state.get(2));
}

void LightController::print_state(const LightState& state) const {
    std::ostringstream ss;
    ss << "LIGHTS:";
    for (int i = 0; i < LightState::NUM_LIGHTS; ++i)
        ss << "  L" << (i+1) << "=" << (state.get(i) ? "ON " : "off");
    logger::info(ss.str());
}

} // namespace gesture