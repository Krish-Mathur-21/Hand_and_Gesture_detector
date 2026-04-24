#define NOMINMAX
#define _USE_MATH_DEFINES
#include "hand_detector.hpp"
#include "logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace gesture {

HandDetector::HandDetector(const Config& config) : config_(config) {}

// Angle at vertex B in triangle ABC (in degrees)
static float angle_between(cv::Point a, cv::Point b, cv::Point c) {
    float ab_x = a.x - b.x, ab_y = a.y - b.y;
    float cb_x = c.x - b.x, cb_y = c.y - b.y;
    float dot   = ab_x * cb_x + ab_y * cb_y;
    float mag_a = std::sqrt(ab_x * ab_x + ab_y * ab_y);
    float mag_c = std::sqrt(cb_x * cb_x + cb_y * cb_y);
    if (mag_a < 1e-5f || mag_c < 1e-5f) return 180.0f;
    float cosine = std::clamp(dot / (mag_a * mag_c), -1.0f, 1.0f);
    return std::acos(cosine) * 180.0f / static_cast<float>(M_PI);
}

int HandDetector::count_fingers(const std::vector<cv::Point>& contour) const {
    if (contour.size() < 20) return 0;

    // Build convex hull indices
    std::vector<int> hull_idx;
    cv::convexHull(contour, hull_idx, false);
    if (hull_idx.size() < 3) return 0;

    std::vector<cv::Vec4i> defects;
    cv::convexityDefects(contour, hull_idx, defects);
    if (defects.empty()) return 0;

    // Use bounding box diagonal as reference size for depth threshold
    cv::Rect bbox    = cv::boundingRect(contour);
    float ref_size   = std::sqrt(static_cast<float>(bbox.width * bbox.width +
                                                     bbox.height * bbox.height));
    float min_depth  = ref_size * 0.15f;  // 15% of hand diagonal = real gap

    int fingers = 0;
    for (const auto& d : defects) {
        cv::Point start  = contour[d[0]];
        cv::Point end    = contour[d[1]];
        cv::Point far    = contour[d[2]];
        float     depth  = d[3] / 256.0f;

        if (depth < min_depth) continue;   // too shallow — noise/wrinkle

        // The angle at the valley point must be acute (< 90°) for a real finger gap
        float ang = angle_between(start, far, end);
        if (ang > 90.0f) continue;         // blunt corner — not a finger gap

        // Valley must be in lower 80% of bounding box (not at top = likely face)
        if (far.y < bbox.y + bbox.height * 0.2f) continue;

        fingers++;
    }

    return std::min(fingers + 1, 5);
}

std::optional<HandDetector::DetectionResult>
HandDetector::detect(const cv::Mat& frame) {
    if (frame.empty()) return std::nullopt;

    cv::Mat mask     = create_skin_mask(frame);
    auto contour_opt = find_largest_contour(mask);
    if (!contour_opt) return std::nullopt;

    const auto& contour = *contour_opt;
    double area       = cv::contourArea(contour);
    double frame_area = static_cast<double>(frame.cols * frame.rows);
    if (area < frame_area * config_.min_contour_ratio) return std::nullopt;

    cv::Rect bbox = cv::boundingRect(contour);

    // Reject detections sitting entirely in top 25% of frame (face area)
    if (bbox.y + bbox.height < frame.rows * 0.25) return std::nullopt;

    // Also reject if bounding box is very wide relative to height (likely shoulders/torso)
    if (static_cast<float>(bbox.width) / bbox.height > 2.5f) return std::nullopt;

    int pad     = config_.padding;
    bbox.x      = (std::max)(0,             bbox.x - pad);
    bbox.y      = (std::max)(0,             bbox.y - pad);
    bbox.width  = (std::min)(frame.cols - bbox.x, bbox.width  + 2 * pad);
    bbox.height = (std::min)(frame.rows - bbox.y, bbox.height + 2 * pad);

    cv::Mat roi = extract_roi(frame, bbox);

    int raw = count_fingers(contour);
    int smoothed_count = smoothed_finger_count(raw);

    DetectionResult result;
    result.roi          = roi;
    result.bounding_box = bbox;
    result.mask         = mask;
    result.contour_area = area;
    result.finger_count = smoothed_count;
    return result;
}

int HandDetector::smoothed_finger_count(int raw_count) {
    finger_buf_.push_back(raw_count);
    if (finger_buf_.size() > 5) finger_buf_.pop_front();

    std::vector<int> sorted(finger_buf_.begin(), finger_buf_.end());
    std::sort(sorted.begin(), sorted.end());
    return sorted[sorted.size() / 2];
}

cv::Mat HandDetector::create_skin_mask(const cv::Mat& frame) const {
    cv::Mat blurred, ycrcb, mask;
    cv::GaussianBlur(frame, blurred,
        cv::Size(config_.blur_kernel_size, config_.blur_kernel_size), 0);
    cv::cvtColor(blurred, ycrcb, cv::COLOR_BGR2YCrCb);
    cv::inRange(ycrcb, config_.lower_skin, config_.upper_skin, mask);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE,
        cv::Size(config_.morph_kernel_size, config_.morph_kernel_size));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel, cv::Point(-1,-1), 2);
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN,  kernel, cv::Point(-1,-1), 1);
    cv::dilate(mask, mask, kernel, cv::Point(-1,-1), 1);
    return mask;
}

std::optional<std::vector<cv::Point>>
HandDetector::find_largest_contour(const cv::Mat& mask) const {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if (contours.empty()) return std::nullopt;
    auto largest = std::max_element(contours.begin(), contours.end(),
        [](const auto& a, const auto& b){
            return cv::contourArea(a) < cv::contourArea(b); });
    return *largest;
}

cv::Mat HandDetector::extract_roi(const cv::Mat& frame,
                                   const cv::Rect& bbox) const {
    cv::Mat roi = frame(bbox).clone();
    cv::Mat resized;
    cv::resize(roi, resized,
               cv::Size(config_.roi_size, config_.roi_size), 0, 0, cv::INTER_AREA);
    return resized;
}

void HandDetector::draw_debug(cv::Mat& frame,
                               const DetectionResult& result) const {
    cv::rectangle(frame, result.bounding_box, cv::Scalar(0, 255, 0), 2);
    cv::putText(frame,
        "Fingers: " + std::to_string(result.finger_count),
        cv::Point(result.bounding_box.x, result.bounding_box.y - 10),
        cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 255), 2);
}

} // namespace gesture