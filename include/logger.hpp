#pragma once
#define NOMINMAX
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace logger {

inline std::string timestamp() {
    auto now  = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &time);
    std::ostringstream ss;
    ss << std::setfill('0')
       << std::setw(2) << tm.tm_hour << ":"
       << std::setw(2) << tm.tm_min  << ":"
       << std::setw(2) << tm.tm_sec;
    return ss.str();
}

template<typename... Args>
inline std::string format(const char* fmt, Args&&... args) {
    std::ostringstream oss;
    std::string f(fmt);
    std::size_t pos = 0;
    (void)std::initializer_list<int>{([&]{
        auto p = f.find("{}", pos);
        if (p != std::string::npos) {
            oss << f.substr(pos, p - pos) << args;
            pos = p + 2;
        }
    }(), 0)...};
    oss << f.substr(pos);
    return oss.str();
}

template<typename... Args>
inline void info(const char* fmt, Args&&... args) {
    std::cout << "[" << timestamp() << "] [info]  "
              << format(fmt, std::forward<Args>(args)...) << "\n";
}
template<typename... Args>
inline void warn(const char* fmt, Args&&... args) {
    std::cout << "[" << timestamp() << "] [warn]  "
              << format(fmt, std::forward<Args>(args)...) << "\n";
}
template<typename... Args>
inline void error(const char* fmt, Args&&... args) {
    std::cerr << "[" << timestamp() << "] [error] "
              << format(fmt, std::forward<Args>(args)...) << "\n";
}

inline void info (const char* msg)        { std::cout << "[" << timestamp() << "] [info]  " << msg << "\n"; }
inline void warn (const char* msg)        { std::cout << "[" << timestamp() << "] [warn]  " << msg << "\n"; }
inline void error(const char* msg)        { std::cerr << "[" << timestamp() << "] [error] " << msg << "\n"; }
inline void info (const std::string& msg) { info(msg.c_str());  }
inline void warn (const std::string& msg) { warn(msg.c_str());  }
inline void error(const std::string& msg) { error(msg.c_str()); }

} // namespace logger