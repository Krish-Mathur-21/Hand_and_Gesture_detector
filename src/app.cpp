#define NOMINMAX
#include "app.hpp"
#include "logger.hpp"
#include <opencv2/opencv.hpp>

namespace gesture {

App::App(const AppConfig& config)
    : config_(config)
    , camera_(config.camera_id, config.frame_width, config.frame_height)
    , classifier_(config.model_path, config.confidence_threshold, config.smoothing_window)
{}

App::~App() { stop(); }

bool App::initialize() {
    logger::info("============================================");
    logger::info("  Hand Gesture Light Control v1.0");
    logger::info("============================================");

    if (!camera_.open()) {
        logger::error("Failed to initialize camera");
        return false;
    }

    // Model is optional — we use geometric finger counting as primary method
    if (classifier_.is_loaded())
        logger::info("Model loaded: " + classifier_.device_string());
    else
        logger::info("No model loaded — using geometric finger counting");

    LightController::Config lc_config;
    lc_config.mode = LightController::Mode::SIMULATION;

    if (!config_.serial_port.empty()) {
        lc_config.mode        = LightController::Mode::SERIAL;
        lc_config.serial_port = config_.serial_port;
        lc_config.baud_rate   = config_.serial_baud;
    }

    light_controller_ = LightController(lc_config);

    if (!light_controller_.initialize()) {
        logger::error("Failed to initialize light controller");
        return false;
    }

    logger::info("============================================");
    logger::info("  All systems initialized!");
    logger::info("  Camera: " + std::to_string(camera_.width()) + "x" + std::to_string(camera_.height()));
    logger::info("  Debug mode: " + std::string(config_.debug_mode ? "ON" : "OFF"));
    logger::info("============================================");
    logger::info("  Controls:");
    logger::info("    Q / ESC  - Quit");
    logger::info("    D        - Toggle debug mode");
    logger::info("    M        - Toggle mirror");
    logger::info("============================================");

    return true;
}

void App::run() {
    running_ = true;
    last_frame_time_ = std::chrono::steady_clock::now();

    while (running_) {
        auto frame_opt = camera_.grab_frame();
        if (!frame_opt) {
            logger::warn("Failed to grab frame");
            continue;
        }

        cv::Mat frame = std::move(*frame_opt);
        if (config_.mirror) cv::flip(frame, frame, 1);

        process_frame(frame);
        current_fps_ = calculate_fps();

        // HUD uses last known gesture stored in last_gesture_
        draw_hud(frame, last_gesture_, current_fps_);

        cv::imshow("Hand Gesture Light Control", frame);

        int key = cv::waitKey(1) & 0xFF;
        switch (key) {
            case 'q': case 27: stop(); break;
            case 'd':
                config_.debug_mode = !config_.debug_mode;
                logger::info("Debug mode: " + std::string(config_.debug_mode ? "ON" : "OFF"));
                break;
            case 'm':
                config_.mirror = !config_.mirror;
                logger::info("Mirror: " + std::string(config_.mirror ? "ON" : "OFF"));
                break;
        }
    }

    cv::destroyAllWindows();
}

void App::process_frame(cv::Mat& frame) {
    auto detection = hand_detector_.detect(frame);

    if (!detection) {
        if (config_.debug_mode)
            cv::putText(frame, "No hand detected", cv::Point(10, 30),
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
        last_gesture_ = {};   // reset to NONE when no hand
        return;
    }

    // Always show the detection box in yellow.
    cv::rectangle(frame, detection->bounding_box, cv::Scalar(0, 255, 255), 2);

    // Show detected finger count above the yellow box.
    cv::putText(frame,
                "Fingers: " + std::to_string(detection->finger_count),
                cv::Point(detection->bounding_box.x, (std::max)(20, detection->bounding_box.y - 10)),
                cv::FONT_HERSHEY_SIMPLEX,
                0.7,
                cv::Scalar(0, 255, 255),
                2);

    // Map finger count → gesture (no model required)
    GestureResult result;
    result.confidence = 1.0f;
    switch (detection->finger_count) {
        case 0: result.gesture = GestureType::FIST;          break;
        case 1: result.gesture = GestureType::ONE_FINGER;    break;
        case 2: result.gesture = GestureType::TWO_FINGERS;   break;
        case 3: result.gesture = GestureType::THREE_FINGERS; break;
        case 4:
        case 5: result.gesture = GestureType::OPEN_PALM;     break;
        default: result.gesture = GestureType::NONE;
    }

    last_gesture_ = result;
    light_controller_.process_gesture(result);
}

void App::draw_hud(cv::Mat& frame, const GestureResult& result, double fps) {
    int y = frame.rows - 10;

    cv::rectangle(frame, cv::Point(0, y - 55), cv::Point(frame.cols, frame.rows),
                  cv::Scalar(0, 0, 0), cv::FILLED);

    cv::putText(frame, "FPS: " + std::to_string(static_cast<int>(fps)),
                cv::Point(10, y - 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);

    std::string gesture_text = "Gesture: " + std::string(gesture_name(result.gesture));
    cv::putText(frame, gesture_text, cv::Point(10, y - 8),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

    cv::putText(frame, "Mode: Geometric",
                cv::Point(250, y - 8),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 200, 200), 1);
}

double App::calculate_fps() {
    auto now       = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - last_frame_time_).count();
    last_frame_time_ = now;
    return elapsed > 0 ? 1.0 / elapsed : 0.0;
}

void App::stop() {
    running_ = false;
    logger::info("Shutting down...");
}

} // namespace gesture