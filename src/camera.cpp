#define NOMINMAX
#include "camera.hpp"
#include "logger.hpp"

namespace gesture {

Camera::Camera(int device_id, int width, int height)
    : device_id_(device_id), width_(width), height_(height) {}

Camera::~Camera() { close(); }

Camera::Camera(Camera&& other) noexcept
    : cap_(std::move(other.cap_))
    , device_id_(other.device_id_)
    , width_(other.width_)
    , height_(other.height_) {}

Camera& Camera::operator=(Camera&& other) noexcept {
    if (this != &other) {
        close();
        cap_       = std::move(other.cap_);
        device_id_ = other.device_id_;
        width_     = other.width_;
        height_    = other.height_;
    }
    return *this;
}

bool Camera::open() {
    logger::info("Opening camera device {}...", device_id_);
    cap_.open(device_id_, cv::CAP_DSHOW);
    if (!cap_.isOpened()) {
        logger::warn("DirectShow failed, trying default backend...");
        cap_.open(device_id_);
    }
    if (!cap_.isOpened()) {
        logger::error("Failed to open camera device {}", device_id_);
        return false;
    }
    cap_.set(cv::CAP_PROP_FRAME_WIDTH,  width_);
    cap_.set(cv::CAP_PROP_FRAME_HEIGHT, height_);
    cap_.set(cv::CAP_PROP_FPS,          30.0);
    cap_.set(cv::CAP_PROP_AUTOFOCUS,    0);
    width_  = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_WIDTH));
    height_ = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT));
    logger::info("Camera opened {}x{} @ {:.0f} FPS",
                 width_, height_, cap_.get(cv::CAP_PROP_FPS));
    return true;
}

void Camera::close() {
    if (cap_.isOpened()) { cap_.release(); logger::info("Camera closed."); }
}

bool Camera::is_open() const { return cap_.isOpened(); }

std::optional<cv::Mat> Camera::grab_frame() {
    cv::Mat frame;
    if (!cap_.isOpened() || !cap_.read(frame) || frame.empty())
        return std::nullopt;
    return frame;
}

int    Camera::width()  const { return width_;  }
int    Camera::height() const { return height_; }
double Camera::fps()    const {
    return cap_.isOpened() ? cap_.get(cv::CAP_PROP_FPS) : 0.0;
}

} // namespace gesture