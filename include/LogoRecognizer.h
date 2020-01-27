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

    // low-pass convolution filter for noise reduction
    cv::Mat smooth(const cv::Mat& I) {
        CV_Assert(I.depth() != sizeof(uchar));

        double temp = 0;
        cv::Mat first(I.rows, I.cols, CV_8UC3);
        cv::Mat_<cv::Vec3b> _I = I;
        cv::Mat_<cv::Vec3b> _R = first;
        for (int i = 1; i < _I.rows - 1; ++i) {
            for (int j = 1; j < _I.cols - 1; ++j) {
                for (int c = 0; c < _I.channels(); ++c) {
                    _R.at<cv::Vec3b>(i, j)[c] = 0;
                    temp = 0;
                    for (int k = -1; k < 2; ++k) {
                        for (int l = -1; l < 2; ++l) {
                            temp +=
                                    (_I.at<cv::Vec3b>(i + k, j + l)[c] * (SMOOTHING_KERNEL[k + 1][l + 1]) );
                        }
                    }
                    _R.at<cv::Vec3b>(i, j)[c] = normalizePixel(temp);
                }
            }
        }
        return _R;
    }

    // high-pass convolution filter for edge sharpening
    cv::Mat sharpen(const cv::Mat& I) {
        CV_Assert(I.depth() != sizeof(uchar));

        double temp = 0;
        cv::Mat first(I.rows, I.cols, CV_8UC3);
        cv::Mat_<cv::Vec3b> _I = I;
        cv::Mat_<cv::Vec3b> _R = first;
        for (int i = 1; i < _I.rows - 1; ++i) {
            for (int j = 1; j < _I.cols - 1; ++j) {
                for (int c = 0; c < _I.channels(); ++c) {
                    _R.at<cv::Vec3b>(i, j)[c] = 0;
                    temp = 0;
                    for (int k = -1; k < 2; ++k) {
                        for (int l = -1; l < 2; ++l) {
                            temp +=
                                    (_I.at<cv::Vec3b>(i + k, j + l)[c] * (SHARPENING_KERNEL[k + 1][l + 1]));
                        }
                    }
                    _R.at<cv::Vec3b>(i, j)[c] = normalizePixel(temp);
                }
            }
        }
        return _R;
    }

    LogoRecognizer() = default;
    explicit LogoRecognizer(const std::string &path);

    void inRange(const cv::Scalar &lower, const cv::Scalar &upper);
    void floodFillAll(const cv::Vec3b &color_to_be_filled, int minimal_object_area = 0);
    void calculateFeatures();
    void printFeatures();
    void analyzeFeatures();
    void showAllAndWait();

    cv::Mat original_im_;
    cv::Mat hsv_im_;
    cv::Mat thresholded_im_;
    cv::Mat flood_filled_im_;

    std::vector<Segment> segments_;

private:
    Segment floodFill(cv::Mat &I, int i, int j, const cv::Vec3b &new_color);
};


#endif //POBR_LOGORECOGNIZER_H
