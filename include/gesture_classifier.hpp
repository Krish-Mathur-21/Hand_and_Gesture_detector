#pragma once

#include "gesture_types.hpp"

#include <torch/script.h>
#include <opencv2/opencv.hpp>

#include <string>
#include <vector>
#include <deque>
#include <mutex>

namespace gesture {

// ============================================================
// Loads a TorchScript model and classifies hand gesture images
// ============================================================
class GestureClassifier {
public:
    explicit GestureClassifier(const std::string& model_path,
                                float confidence_threshold = 0.7f,
                                int smoothing_window = 5);

    // Load / reload the model
    bool load_model(const std::string& model_path);
    bool is_loaded() const;

    // Classify a preprocessed hand ROI image (BGR, 128x128)
    GestureResult classify(const cv::Mat& hand_roi);

    // Get smoothed result (majority vote over recent frames)
    GestureResult smoothed_result() const;

    // Configuration
    void set_confidence_threshold(float threshold);
    void set_smoothing_window(int window_size);

    // Device info
    std::string device_string() const;

private:
    // Preprocess OpenCV Mat → Torch Tensor
    torch::Tensor preprocess(const cv::Mat& image) const;

    // Apply softmax and extract top prediction
    GestureResult postprocess(const torch::Tensor& output) const;

    // Update smoothing buffer
    void update_buffer(const GestureResult& result);

    torch::jit::script::Module model_;
    torch::Device              device_;
    bool                       loaded_ = false;

    float confidence_threshold_;
    int   smoothing_window_;

    mutable std::mutex          buffer_mutex_;
    std::deque<GestureResult>   result_buffer_;

    // Normalization constants (ImageNet defaults)
    static constexpr float MEAN[] = {0.485f, 0.456f, 0.406f};
    static constexpr float STD[]  = {0.229f, 0.224f, 0.225f};
    static constexpr int   INPUT_SIZE = 128;
};

} // namespace gesture