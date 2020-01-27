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

    bool isLetterC();
    bool isLetterI();
    bool isLetterT();

    std::vector<cv::Point2i> pixels_;
    cv::Scalar color_;

    double x_{-1};
    double y_{-1};

    int max_x_{0};
    int min_x_{1000};
    int max_y_{0};
    int min_y_{1000};

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

inline
bool areConsecutiveLetters(const Segment &left, const Segment &right)
{
    return ((left.max_x_ < right.min_x_ or std::fabs(left.max_x_ - right.min_x_) < 10) and
            (right.min_x_ - left.max_x_) < 200 and
            right.max_y_ > left.min_y_ and right.min_y_ < left.max_y_);
}

inline
std::pair<cv::Point2i, cv::Point2i> getBox(std::vector<Segment> & segments)
{
    cv::Point2i left_upper(10000, 10000);
    cv::Point2i right_lower{0, 0};

    for (const auto & segment : segments)
    {
        left_upper.x = std::min(left_upper.x, segment.min_x_);
        left_upper.y = std::min(left_upper.y, segment.min_y_);
        right_lower.x = std::max(right_lower.x, segment.max_x_);
        right_lower.y = std::max(right_lower.y, segment.max_y_);
    }

    return {left_upper, right_lower};
}

#endif //POBR_SEGMENT_H
