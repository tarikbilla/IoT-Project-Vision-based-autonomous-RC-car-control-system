/**
 * @file camera_capture.cpp
 * @brief Implementation of camera capture module for real-time video frame acquisition
 */

#include "camera_capture.h"
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <string>

namespace rc_car {

CameraCapture::CameraCapture() 
    : running_(false), paused_(false),
      camera_index_(0), target_width_(1920), target_height_(1080), target_fps_(30) {
}

CameraCapture::CameraCapture(int camera_index)
    : running_(false), paused_(false),
      camera_index_(camera_index), target_width_(1920), target_height_(1080), target_fps_(30) {
}

CameraCapture::~CameraCapture() {
    stop();
    if (cap_.isOpened()) {
        cap_.release();
    }
}

bool CameraCapture::initialize(int camera_index, int width, int height, int fps) {
    camera_index_ = camera_index;
    target_width_ = width;
    target_height_ = height;
    target_fps_ = fps;
    
    if (cap_.isOpened()) {
        cap_.release();
    }
    
    // Open camera - try V4L2 backend first (more reliable on Linux)
    // Format: "v4l2:///dev/video0" or just index
    std::string camera_path = "v4l2:///dev/video" + std::to_string(camera_index);
    
    // Try V4L2 backend first
    cap_.open(camera_path, cv::CAP_V4L2);
    
    // If V4L2 fails, try default backend
    if (!cap_.isOpened()) {
        std::cerr << "Warning: V4L2 backend failed, trying default backend..." << std::endl;
        cap_.open(camera_index);
    }
    
    // If still fails, try direct device path
    if (!cap_.isOpened()) {
        std::string dev_path = "/dev/video" + std::to_string(camera_index);
        std::cerr << "Warning: Default backend failed, trying direct path: " << dev_path << std::endl;
        cap_.open(dev_path, cv::CAP_V4L2);
    }
    
    if (!cap_.isOpened()) {
        std::cerr << "Error: Could not open camera " << camera_index << std::endl;
        std::cerr << "Tried: v4l2:///dev/video" << camera_index << ", index " << camera_index 
                  << ", and /dev/video" << camera_index << std::endl;
        std::cerr << "Please check:" << std::endl;
        std::cerr << "  1. Camera is connected and powered on" << std::endl;
        std::cerr << "  2. User has video group permissions: sudo usermod -a -G video $USER" << std::endl;
        std::cerr << "  3. Camera device exists: ls -l /dev/video*" << std::endl;
        return false;
    }
    
    // Set properties
    cap_.set(cv::CAP_PROP_FRAME_WIDTH, target_width_);
    cap_.set(cv::CAP_PROP_FRAME_HEIGHT, target_height_);
    cap_.set(cv::CAP_PROP_FPS, target_fps_);
    
    // Verify actual resolution
    int actual_width = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_WIDTH));
    int actual_height = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT));
    int actual_fps = static_cast<int>(cap_.get(cv::CAP_PROP_FPS));
    
    std::cout << "Camera initialized: " << actual_width << "x" << actual_height 
              << " @ " << actual_fps << " FPS" << std::endl;
    
    return true;
}

bool CameraCapture::initialize(const std::string& video_source) {
    if (cap_.isOpened()) {
        cap_.release();
    }
    
    // Try to open as video file or stream URL
    cap_.open(video_source);
    if (!cap_.isOpened()) {
        std::cerr << "Error: Could not open video source: " << video_source << std::endl;
        return false;
    }
    
    target_width_ = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_WIDTH));
    target_height_ = static_cast<int>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT));
    target_fps_ = static_cast<int>(cap_.get(cv::CAP_PROP_FPS));
    
    std::cout << "Video source initialized: " << target_width_ << "x" << target_height_ 
              << " @ " << target_fps_ << " FPS" << std::endl;
    
    return true;
}

bool CameraCapture::start() {
    if (!cap_.isOpened()) {
        std::cerr << "Error: Camera not initialized" << std::endl;
        return false;
    }
    
    if (running_) {
        return true;  // Already running
    }
    
    running_ = true;
    paused_ = false;
    capture_thread_ = std::thread(&CameraCapture::captureLoop, this);
    
    return true;
}

void CameraCapture::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    if (capture_thread_.joinable()) {
        capture_thread_.join();
    }
}

void CameraCapture::pause() {
    paused_ = true;
}

void CameraCapture::resume() {
    paused_ = false;
}

void CameraCapture::captureLoop() {
    cv::Mat frame;
    auto frame_time = std::chrono::milliseconds(1000 / target_fps_);
    
    while (running_) {
        if (paused_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        auto start_time = std::chrono::steady_clock::now();
        
        // Attempt to read frame from camera
        if (!cap_.read(frame)) {
            std::cerr << "Warning: Failed to read frame from camera (attempt " 
                      << " - camera may be disconnected)" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Check if camera is still opened
            if (!cap_.isOpened()) {
                std::cerr << "Error: Camera connection lost. Stopping capture." << std::endl;
                running_ = false;
                break;
            }
            continue;
        }
        
        // Validate frame
        if (frame.empty()) {
            std::cerr << "Warning: Received empty frame" << std::endl;
            continue;
        }
        
        // Resize if needed
        if (frame.cols != target_width_ || frame.rows != target_height_) {
            cv::resize(frame, frame, cv::Size(target_width_, target_height_));
        }
        
        // Update current frame (thread-safe)
        {
            std::lock_guard<std::mutex> lock(frame_mutex_);
            frame.copyTo(current_frame_);
        }
        
        // Maintain frame rate
        auto elapsed = std::chrono::steady_clock::now() - start_time;
        auto sleep_time = frame_time - elapsed;
        if (sleep_time.count() > 0) {
            std::this_thread::sleep_for(sleep_time);
        }
    }
}

bool CameraCapture::getFrame(cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    if (current_frame_.empty()) {
        return false;
    }
    current_frame_.copyTo(frame);
    return true;
}

void CameraCapture::setResolution(int width, int height) {
    target_width_ = width;
    target_height_ = height;
    if (cap_.isOpened()) {
        cap_.set(cv::CAP_PROP_FRAME_WIDTH, width);
        cap_.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    }
}

void CameraCapture::setFPS(int fps) {
    target_fps_ = fps;
    if (cap_.isOpened()) {
        cap_.set(cv::CAP_PROP_FPS, fps);
    }
}

} // namespace rc_car
