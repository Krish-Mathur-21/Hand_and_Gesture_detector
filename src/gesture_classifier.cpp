#define NOMINMAX
#include "gesture_classifier.hpp"
#include "logger.hpp"
#include <algorithm>
#include <numeric>
#include <map>

namespace gesture {

constexpr float GestureClassifier::MEAN[];
constexpr float GestureClassifier::STD[];

GestureClassifier::GestureClassifier(const std::string& model_path,
                                     float confidence_threshold,
                                     int   smoothing_window)
    : device_(torch::kCPU)
    , confidence_threshold_(confidence_threshold)
    , smoothing_window_(smoothing_window)
{
    logger::info("GestureClassifier: using CPU.");
    if (!model_path.empty()) load_model(model_path);
}

bool GestureClassifier::load_model(const std::string& model_path) {
    try {
        logger::info("Loading TorchScript model from {}", model_path);
        model_ = torch::jit::load(model_path, device_);
        model_.eval();
        torch::NoGradGuard ng;
        auto dummy = torch::zeros({1, 3, INPUT_SIZE, INPUT_SIZE});
        model_.forward({dummy});
        loaded_ = true;
        logger::info("Model loaded successfully on CPU.");
        return true;
    } catch (const c10::Error& e) {
        logger::error("Failed to load model: " + std::string(e.what()));
        loaded_ = false; return false;
    } catch (const std::exception& e) {
        logger::error("Failed to load model: " + std::string(e.what()));
        loaded_ = false; return false;
    }
}

bool GestureClassifier::is_loaded() const { return loaded_; }

GestureResult GestureClassifier::classify(const cv::Mat& hand_roi) {
    if (!loaded_) { logger::warn("Model not loaded, returning NONE"); return {}; }
    if (hand_roi.empty()) return {};
    try {
        torch::Tensor input = preprocess(hand_roi);
        torch::NoGradGuard ng;
        auto output = model_.forward({input}).toTensor();
        GestureResult r = postprocess(output);
        update_buffer(r);
        return r;
    } catch (const std::exception& e) {
        logger::error("Classification error: " + std::string(e.what()));
        return {};
    }
}

torch::Tensor GestureClassifier::preprocess(const cv::Mat& image) const {
    cv::Mat resized, rgb, flt;
    if (image.cols != INPUT_SIZE || image.rows != INPUT_SIZE)
        cv::resize(image, resized, cv::Size(INPUT_SIZE, INPUT_SIZE));
    else
        resized = image;
    cv::cvtColor(resized, rgb, cv::COLOR_BGR2RGB);
    rgb.convertTo(flt, CV_32F, 1.0 / 255.0);
    auto t = torch::from_blob(
        flt.data, {1, INPUT_SIZE, INPUT_SIZE, 3}, torch::kFloat32).clone();
    t = t.permute({0, 3, 1, 2});
    t[0][0] = (t[0][0] - MEAN[0]) / STD[0];
    t[0][1] = (t[0][1] - MEAN[1]) / STD[1];
    t[0][2] = (t[0][2] - MEAN[2]) / STD[2];
    return t;
}

GestureResult GestureClassifier::postprocess(const torch::Tensor& output) const {
    auto probs = torch::softmax(output, 1);
    auto [max_prob, max_idx] = probs[0].max(0);
    GestureResult r;
    int idx = max_idx.item<int>();
    r.confidence = max_prob.item<float>();
    r.gesture = (idx >= 0 && idx < static_cast<int>(GestureType::COUNT))
                ? static_cast<GestureType>(idx) : GestureType::NONE;
    return r;
}

void GestureClassifier::update_buffer(const GestureResult& r) {
    std::lock_guard<std::mutex> lk(buffer_mutex_);
    result_buffer_.push_back(r);
    while (static_cast<int>(result_buffer_.size()) > smoothing_window_)
        result_buffer_.pop_front();
}

GestureResult GestureClassifier::smoothed_result() const {
    std::lock_guard<std::mutex> lk(buffer_mutex_);
    if (result_buffer_.empty()) return {};
    std::map<GestureType, int>   votes;
    std::map<GestureType, float> conf_sum;
    for (const auto& r : result_buffer_)
        if (r.is_valid(confidence_threshold_)) {
            votes[r.gesture]++;
            conf_sum[r.gesture] += r.confidence;
        }
    if (votes.empty()) return {};
    auto best = std::max_element(votes.begin(), votes.end(),
        [](const auto& a, const auto& b){ return a.second < b.second; });
    GestureResult s;
    s.gesture    = best->first;
    s.confidence = conf_sum[best->first] / static_cast<float>(best->second);
    return s;
}

void GestureClassifier::set_confidence_threshold(float t) {
    confidence_threshold_ = std::clamp(t, 0.0f, 1.0f);
}
void GestureClassifier::set_smoothing_window(int w) {
    smoothing_window_ = (std::max)(1, w);
}
std::string GestureClassifier::device_string() const { return "CPU"; }

} // namespace gesture