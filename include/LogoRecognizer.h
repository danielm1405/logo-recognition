//
// Created by dan on 02.01.2020.
//

#ifndef POBR_LOGORECOGNIZER_H
#define POBR_LOGORECOGNIZER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Segment.h"

class LogoRecognizer
{
public:
    LogoRecognizer() = default;
    explicit LogoRecognizer(const std::string &path);

    // low-pass convolution filter for noise reduction
    void smooth();
    // high-pass convolution filter for edge sharpening
    void sharpen();

    double getMeanSaturation();

    void inRange(const cv::Scalar &lower, const cv::Scalar &upper);
    void floodFillAll(const cv::Vec3b &color_to_be_filled, int minimal_object_area = 0);
    void calculateFeatures();
    void printFeatures();
    void analyzeFeatures();
    void showAllAndWait();

    cv::Mat original_im_;
    cv::Mat processed_im_;
    cv::Mat hsv_im_;
    cv::Mat thresholded_im_;
    cv::Mat flood_filled_im_;

    std::vector<Segment> segments_;

private:
    Segment floodFill(cv::Mat &I, int i, int j, const cv::Vec3b &new_color);

    const double SMOOTHING_KERNEL[3][3] = {
            {1./9, 1./9, 1./9},
            {1./9, 1./9, 1./9},
            {1./9, 1./9, 1./9}
    };

    const double SHARPENING_KERNEL[3][3] = {
            {0, -1, 0},
            {-1, 5, -1},
            {0, -1, 0},
    };
};


#endif //POBR_LOGORECOGNIZER_H
