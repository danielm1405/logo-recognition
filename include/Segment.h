//
// Created by dan on 02.01.2020.
//

#ifndef POBR_SEGMENT_H
#define POBR_SEGMENT_H

#include <iostream>

#include "opencv2/core/core.hpp"

#include "Utils.h"


class Segment {
public:
    explicit Segment() = default;
    explicit Segment(const cv::Scalar &color) : color_(color) {};

    void calculateFeatures();
    void printAllFeatures();

    double isLetterCWithErrorPercent();

    std::vector<cv::Point2i> pixels_;
    cv::Scalar color_;

    double x_{-1};
    double y_{-1};

    double m00_{0};
    double m10_{0};
    double m01_{0};
    double m11_{0};
    double m20_{0};
    double m02_{0};
    double m21_{0};
    double m12_{0};
    double m30_{0};
    double m03_{0};

    double M00_{0};
    double M11_{0};
    double M20_{0};
    double M02_{0};
    double M21_{0};
    double M12_{0};
    double M30_{0};
    double M03_{0};

    double M1_{0};
    double M2_{0};
    double M3_{0};
    double M4_{0};
    double M7_{0};
};

#endif //POBR_SEGMENT_H
