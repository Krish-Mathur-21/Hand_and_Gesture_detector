# # # cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
# # # project(hand_gesture_detection)

# # # set(CMAKE_CXX_STANDARD 17)
# # # set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # # set(Torch_DIR   "C:/Libandtools/libtorch/share/cmake/Torch")
# # # set(OpenCV_DIR  "C:/Libandtools/opencv/build/x64/vc16/lib")

# # # list(APPEND CMAKE_PREFIX_PATH
# # #     "C:/Libandtools/libtorch"
# # #     "C:/Libandtools/opencv/build"
# # # )

# # # find_package(Torch  REQUIRED)
# # # find_package(OpenCV REQUIRED)

# # # include_directories(${OpenCV_INCLUDE_DIRS})
# # # include_directories(${PROJECT_SOURCE_DIR}/include)

# # # add_executable(hand_gesture_detection
# # #     src/main.cpp
# # #     src/app.cpp
# # #     src/camera.cpp
# # #     src/hand_detector.cpp
# # #     src/gesture_classifier.cpp
# # #     src/light_controller.cpp
# # # )

# # # target_link_libraries(hand_gesture_detection
# # #     "${TORCH_LIBRARIES}"
# # #     ${OpenCV_LIBS}
# # # )

# # # target_compile_features(hand_gesture_detection PRIVATE cxx_std_17)
# # # set_property(TARGET hand_gesture_detection PROPERTY CXX_STANDARD 17)
# # # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# # # if (MSVC)
# # #     file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
# # #     add_custom_command(TARGET hand_gesture_detection POST_BUILD
# # #         COMMAND ${CMAKE_COMMAND} -E copy_if_different
# # #             ${TORCH_DLLS}
# # #             $<TARGET_FILE_DIR:hand_gesture_detection>)

# # #     add_custom_command(TARGET hand_gesture_detection POST_BUILD
# # #         COMMAND ${CMAKE_COMMAND} -E copy_if_different
# # #             "C:/Libandtools/opencv/build/x64/vc16/bin/opencv_world4130.dll"
# # #             $<TARGET_FILE_DIR:hand_gesture_detection>)
# # # endif()

# # cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# # # vcpkg toolchain — MUST be before project()
# # set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
# #     CACHE STRING "vcpkg toolchain")
# # set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "vcpkg triplet")

# # project(hand_gesture_detection)

# # set(CMAKE_CXX_STANDARD 17)
# # set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # set(Torch_DIR  "C:/Libandtools/libtorch/share/cmake/Torch")
# # set(OpenCV_DIR "C:/Libandtools/opencv/build/x64/vc16/lib")

# # list(APPEND CMAKE_PREFIX_PATH
# #     "C:/Libandtools/libtorch"
# #     "C:/Libandtools/opencv/build"
# # )

# # find_package(Torch  REQUIRED)
# # find_package(OpenCV REQUIRED)
# # find_package(cxxopts CONFIG REQUIRED)

# # # Prevent Windows min/max macros
# # add_compile_definitions(NOMINMAX)

# # include_directories(${OpenCV_INCLUDE_DIRS})
# # include_directories(${PROJECT_SOURCE_DIR}/include)

# # add_executable(hand_gesture_detection
# #     src/main.cpp
# #     src/app.cpp
# #     src/camera.cpp
# #     src/hand_detector.cpp
# #     src/gesture_classifier.cpp
# #     src/light_controller.cpp
# # )

# # target_link_libraries(hand_gesture_detection PRIVATE
# #     "${TORCH_LIBRARIES}"
# #     ${OpenCV_LIBS}
# #     cxxopts::cxxopts
# # )

# # target_compile_features(hand_gesture_detection PRIVATE cxx_std_17)
# # set_property(TARGET hand_gesture_detection PROPERTY CXX_STANDARD 17)
# # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# # if(MSVC)
# #     # /utf-8 fixes fmt/spdlog Unicode assertion on MSVC
# #     # /wd flags silence harmless LibTorch size_t warnings
# #     add_compile_options(/utf-8 /wd4267 /wd4244 /wd4305)

# #     file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
# #     add_custom_command(TARGET hand_gesture_detection POST_BUILD
# #         COMMAND ${CMAKE_COMMAND} -E copy_if_different
# #             ${TORCH_DLLS}
# #             $<TARGET_FILE_DIR:hand_gesture_detection>)

# #     add_custom_command(TARGET hand_gesture_detection POST_BUILD
# #         COMMAND ${CMAKE_COMMAND} -E copy_if_different
# #             "C:/Libandtools/opencv/build/x64/vc16/bin/opencv_world4130.dll"
# #             $<TARGET_FILE_DIR:hand_gesture_detection>)
# # endif()
# cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
#     CACHE STRING "vcpkg toolchain")
# set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "vcpkg triplet")

# project(hand_gesture_detection)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(Torch_DIR  "C:/Libandtools/libtorch/share/cmake/Torch")
# set(OpenCV_DIR "C:/Libandtools/opencv/build/x64/vc16/lib")

# list(APPEND CMAKE_PREFIX_PATH
#     "C:/Libandtools/libtorch"
#     "C:/Libandtools/opencv/build"
# )

# find_package(Torch  REQUIRED)
# find_package(OpenCV REQUIRED)
# find_package(cxxopts CONFIG REQUIRED)

# add_compile_definitions(NOMINMAX)

# include_directories(${OpenCV_INCLUDE_DIRS})
# include_directories(${PROJECT_SOURCE_DIR}/include)

# add_executable(hand_gesture_detection
#     src/main.cpp
#     src/app.cpp
#     src/camera.cpp
#     src/hand_detector.cpp
#     src/gesture_classifier.cpp
#     src/light_controller.cpp
# )

# target_link_libraries(hand_gesture_detection PRIVATE
#     "${TORCH_LIBRARIES}"
#     ${OpenCV_LIBS}
#     cxxopts::cxxopts
# )

# target_compile_features(hand_gesture_detection PRIVATE cxx_std_17)
# set_property(TARGET hand_gesture_detection PROPERTY CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# if(MSVC)
#     add_compile_options(/utf-8 /wd4267 /wd4244 /wd4305)

#     # Copy LibTorch DLLs — use hardcoded path, TORCH_INSTALL_PREFIX is unreliable
#     add_custom_command(TARGET hand_gesture_detection POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#             "C:/Libandtools/libtorch/lib/c10.dll"
#             "C:/Libandtools/libtorch/lib/torch.dll"
#             "C:/Libandtools/libtorch/lib/torch_cpu.dll"
#             "C:/Libandtools/libtorch/lib/torch_global_deps.dll"
#             "C:/Libandtools/libtorch/lib/libiomp5md.dll"
#             "C:/Libandtools/libtorch/lib/libiompstubs5md.dll"
#             "C:/Libandtools/libtorch/lib/uv.dll"
#             $<TARGET_FILE_DIR:hand_gesture_detection>
#         COMMENT "Copying LibTorch DLLs...")

#     # Copy OpenCV DLL
#     add_custom_command(TARGET hand_gesture_detection POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#             "C:/Libandtools/opencv/build/x64/vc16/bin/opencv_world4130.dll"
#             $<TARGET_FILE_DIR:hand_gesture_detection>
#         COMMENT "Copying OpenCV DLL...")
# endif()
cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# Disable vcpkg's applocal.ps1 post-build step — we handle DLL copying ourselves
set(VCPKG_APPLOCAL_DEPS OFF)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL OFF)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "vcpkg toolchain")
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "vcpkg triplet")

project(hand_gesture_detection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Torch_DIR  "C:/Libandtools/libtorch/share/cmake/Torch")
set(OpenCV_DIR "C:/Libandtools/opencv/build/x64/vc16/lib")

list(APPEND CMAKE_PREFIX_PATH
    "C:/Libandtools/libtorch"
    "C:/Libandtools/opencv/build"
)

find_package(Torch  REQUIRED)
find_package(OpenCV REQUIRED)
find_package(cxxopts CONFIG REQUIRED)

add_compile_definitions(NOMINMAX)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(hand_gesture_detection
    src/main.cpp
    src/app.cpp
    src/camera.cpp
    src/hand_detector.cpp
    src/gesture_classifier.cpp
    src/light_controller.cpp
)

target_link_libraries(hand_gesture_detection PRIVATE
    "${TORCH_LIBRARIES}"
    ${OpenCV_LIBS}
    cxxopts::cxxopts
)

target_compile_features(hand_gesture_detection PRIVATE cxx_std_17)
set_property(TARGET hand_gesture_detection PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

if(MSVC)
    add_compile_options(/utf-8 /wd4267 /wd4244 /wd4305)

    # Copy LibTorch DLLs
    add_custom_command(TARGET hand_gesture_detection POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Libandtools/libtorch/lib/c10.dll"
            "C:/Libandtools/libtorch/lib/torch.dll"
            "C:/Libandtools/libtorch/lib/torch_cpu.dll"
            "C:/Libandtools/libtorch/lib/torch_global_deps.dll"
            "C:/Libandtools/libtorch/lib/libiomp5md.dll"
            "C:/Libandtools/libtorch/lib/libiompstubs5md.dll"
            "C:/Libandtools/libtorch/lib/uv.dll"
            $<TARGET_FILE_DIR:hand_gesture_detection>
        COMMENT "Copying LibTorch DLLs...")

    # Copy OpenCV DLL
    add_custom_command(TARGET hand_gesture_detection POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Libandtools/opencv/build/x64/vc16/bin/opencv_world4130.dll"
            $<TARGET_FILE_DIR:hand_gesture_detection>
        COMMENT "Copying OpenCV DLL...")
endif()