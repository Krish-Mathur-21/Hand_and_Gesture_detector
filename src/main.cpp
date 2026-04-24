#define NOMINMAX
#include "app.hpp"
#include "gesture_types.hpp"
#include "logger.hpp"
#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <csignal>

static gesture::App* g_app = nullptr;

void signal_handler(int signal) {
    logger::info("Signal {} received, shutting down...", signal);
    if (g_app) g_app->stop();
}

int main(int argc, char* argv[]) {
    cxxopts::Options options("hand-gesture-light-control",
                             "Hand Gesture Detection to Control Light Bulbs");
    options.add_options()
        ("m,model",     "Path to TorchScript model (.pt)",
                         cxxopts::value<std::string>()->default_value("model/gesture_model.pt"))
        ("c,camera",    "Camera device ID",
                         cxxopts::value<int>()->default_value("0"))
        ("W,width",     "Frame width",    cxxopts::value<int>()->default_value("640"))
        ("H,height",    "Frame height",   cxxopts::value<int>()->default_value("480"))
        ("t,threshold", "Confidence threshold 0.0-1.0",
                         cxxopts::value<float>()->default_value("0.7"))
        ("s,smoothing", "Smoothing window (frames)",
                         cxxopts::value<int>()->default_value("5"))
        ("serial",      "Serial port for Arduino (e.g. COM3)",
                         cxxopts::value<std::string>()->default_value(""))
        ("baud",        "Serial baud rate", cxxopts::value<int>()->default_value("9600"))
        ("d,debug",     "Enable debug mode",
                         cxxopts::value<bool>()->default_value("false"))
        ("no-mirror",   "Disable horizontal mirror",
                         cxxopts::value<bool>()->default_value("false"))
        ("h,help",      "Print help");

    cxxopts::ParseResult parsed;
    try {
        parsed = options.parse(argc, argv);
    } catch (const std::exception& e) {
        logger::error("Argument error: " + std::string(e.what()));
        std::cout << options.help() << std::endl;
        return 1;
    }
    if (parsed.count("help")) { std::cout << options.help() << std::endl; return 0; }

    gesture::AppConfig config;
    config.model_path           = parsed["model"].as<std::string>();
    config.camera_id            = parsed["camera"].as<int>();
    config.frame_width          = parsed["width"].as<int>();
    config.frame_height         = parsed["height"].as<int>();
    config.confidence_threshold = parsed["threshold"].as<float>();
    config.smoothing_window     = parsed["smoothing"].as<int>();
    config.serial_port          = parsed["serial"].as<std::string>();
    config.serial_baud          = parsed["baud"].as<int>();
    config.debug_mode           = parsed["debug"].as<bool>();
    config.mirror               = !parsed["no-mirror"].as<bool>();

    std::signal(SIGINT,  signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        gesture::App app(config);
        g_app = &app;
        if (!app.initialize()) { logger::error("Initialization failed!"); return 1; }
        app.run();
        logger::info("Application exited cleanly.");
        return 0;
    } catch (const std::exception& e) {
        logger::error("Fatal error: " + std::string(e.what()));
        return 1;
    }
}