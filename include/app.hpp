#pragma once

#include "gesture_types.hpp"
#include "camera.hpp"
#include "hand_detector.hpp"
#include "gesture_classifier.hpp"
#include "light_controller.hpp"

#include <atomic>
#include <string>
#include <chrono>

namespace gesture {

class App {
public:
    explicit App(const AppConfig& config);
    ~App();

    bool initialize();
    void run();
    void stop();

private:
    void   process_frame(cv::Mat& frame);
    void   draw_hud(cv::Mat& frame, const GestureResult& result, double fps);
    double calculate_fps();

    AppConfig         config_;
    Camera            camera_;
    HandDetector      hand_detector_;
    GestureClassifier classifier_;
    LightController   light_controller_;

    std::atomic<bool> running_{false};

    std::chrono::steady_clock::time_point last_frame_time_;
    double        current_fps_ = 0.0;
    GestureResult last_gesture_;   // ← stores last detected gesture for HUD
};

} // namespace gesture