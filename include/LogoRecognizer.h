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

    // recognizes the logos on image
    void run(bool verbose = false);
private:
    // low-pass convolution filter for noise reduction
    void smooth();
    // high-pass convolution filter for edge sharpening
    void sharpen();

    double getMeanSaturation();

    // performs thresholding - colours pixel white if in range <lower, higher> and black otherwise
    void inRange(const cv::Scalar &lower, const cv::Scalar &upper);

    // uses flood fill segmentation algorithm to find an object
    Segment floodFill(cv::Mat &I, int i, int j, const cv::Vec3b &new_color);
    // uses flood fill to find all objects (and rejects those which are smaller than minimal area)
    void floodFillAll(const cv::Vec3b &color_to_be_filled, int minimal_object_area = 0);

    // calculates geometrical moments on all found segments
    void calculateFeatures();

    void printFeatures();

    // for each segment checks if it can be classified as 'c', 'i' or 't' letter
    void analyzeFeatures();

    // searches for string 'citi' in classified segments based on their relative positions
    void findLogos();

    void showAllAndWait();

    cv::Mat original_im_;
    cv::Mat processed_im_;

    std::vector<Segment> segments_;

    std::vector<Segment*> c_letters_;
    std::vector<Segment*> i_letters_;
    std::vector<Segment*> t_letters_;

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
