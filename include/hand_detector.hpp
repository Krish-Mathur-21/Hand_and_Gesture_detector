#pragma once
#include <opencv2/opencv.hpp>
#include <optional>
#include <vector>
#include <deque>

namespace gesture {

class HandDetector {
public:
    struct DetectionResult {
        cv::Mat  roi;
        cv::Rect bounding_box;
        cv::Mat  mask;
        double   contour_area = 0;
        int      finger_count = 0;   // ← geometric finger count, no model needed
    };

    struct Config {
        // Tighter YCrCb range — reduces face false positives
        cv::Scalar lower_skin    = cv::Scalar(0,   138, 80);
        cv::Scalar upper_skin    = cv::Scalar(255, 168, 120);
        // Larger minimum area — filters out small face regions
        double min_contour_ratio = 0.04;
        int roi_size             = 128;
        int morph_kernel_size    = 5;
        int blur_kernel_size     = 5;
        int padding              = 30;
    };

    explicit HandDetector(const Config& config = Config{});

    std::optional<DetectionResult> detect(const cv::Mat& frame);
    void draw_debug(cv::Mat& frame, const DetectionResult& result) const;

    Config&       config()       { return config_; }
    const Config& config() const { return config_; }

private:
    cv::Mat create_skin_mask(const cv::Mat& frame) const;
    std::optional<std::vector<cv::Point>> find_largest_contour(const cv::Mat& mask) const;
    cv::Mat extract_roi(const cv::Mat& frame, const cv::Rect& bbox) const;
    int     count_fingers(const std::vector<cv::Point>& contour) const;
    int     smoothed_finger_count(int raw_count);

    Config config_;
    std::deque<int> finger_buf_;
};

} // namespace gesture