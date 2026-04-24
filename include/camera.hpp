#pragma once

#include <opencv2/opencv.hpp>
#include <optional>
#include <string>

namespace gesture {

class Camera {
public:
    explicit Camera(int device_id = 0, int width = 640, int height = 480);
    ~Camera();

    // Non-copyable, movable
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) noexcept;
    Camera& operator=(Camera&&) noexcept;

    bool open();
    void close();
    bool is_open() const;

    // Grab a frame; returns empty optional on failure
    std::optional<cv::Mat> grab_frame();

    // Getters
    int width() const;
    int height() const;
    double fps() const;

private:
    cv::VideoCapture cap_;
    int device_id_;
    int width_;
    int height_;
};

} // namespace gesture