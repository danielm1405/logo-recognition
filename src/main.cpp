#include <iostream>

#include <opencv2/core/core.hpp>

#include "LogoRecognizer.h"


int main(int, char *[]) {
    // load logo image
    LogoRecognizer citi_recognizer = LogoRecognizer("../data/keeps/citi1.jpg");

    // quality improvement
    citi_recognizer.sharpen();
    citi_recognizer.smooth();

    // color space conversion
	cv::cvtColor(citi_recognizer.processed_im_, citi_recognizer.hsv_im_, cv::COLOR_BGR2HSV);

	// adaptive thresholding
    auto mean_saturation = citi_recognizer.getMeanSaturation();

    cv::Scalar lower_limit{100, mean_saturation, 40};
    cv::Scalar upper_limit{130, 255, 180};

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

    // images display
    citi_recognizer.showAllAndWait();

    return 0;
}
