#include <iostream>

#include <opencv2/core/core.hpp>

#include "LogoRecognizer.h"


int main(int, char *[]) {
    // load logo image
    LogoRecognizer citi_recognizer = LogoRecognizer("../data/keeps/citi12.jpg");

    citi_recognizer.original_im_ = citi_recognizer.sharpen(citi_recognizer.original_im_);
    citi_recognizer.original_im_ = citi_recognizer.smooth(citi_recognizer.original_im_);

    // color space conversion
	cv::cvtColor(citi_recognizer.original_im_, citi_recognizer.hsv_im_, cv::COLOR_BGR2HSV);

	// thresholding

//	after 'c'
//    cv::Scalar lower_limit{100, 85, 20};
//    cv::Scalar upper_limit{180, 255, 200};

//  citi1
//  mean_sat = 29.7
//    cv::Scalar lower_limit{100, 40, 40};
//    cv::Scalar upper_limit{150, 255, 180};

//  citi8
//  mean_sat = 86.3
//    cv::Scalar lower_limit{100, 85, 40};
//    cv::Scalar upper_limit{150, 255, 180};

//  citi9
//  mean_sat = 72.6
//    cv::Scalar lower_limit{100, 85, 40};
//    cv::Scalar upper_limit{150, 255, 180};

//  citi12
//  mean_sat = 44.3
//    cv::Scalar lower_limit{100, 75, 40};
//    cv::Scalar upper_limit{150, 255, 180};

    cv::Scalar lower_limit{100, 50, 40};
    cv::Scalar upper_limit{130, 255, 180};
    cv::inRange(citi_recognizer.hsv_im_, lower_limit, upper_limit, citi_recognizer.thresholded_im_);
    citi_recognizer.inRange(lower_limit, upper_limit);

    // segmentation
	cv::Vec3b color_to_fill{255, 255, 255};
	int minimal_object_area{80};
    citi_recognizer.floodFillAll(color_to_fill, minimal_object_area);

    // features calculation
    citi_recognizer.calculateFeatures();
    citi_recognizer.printFeatures();

    // features analysis
    citi_recognizer.analyzeFeatures();

    // show images
    citi_recognizer.showAllAndWait();
    return 0;
}
