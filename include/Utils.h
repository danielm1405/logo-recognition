//
// Created by dan on 02.01.2020.
//

#ifndef POBR_UTILS_H
#define POBR_UTILS_H

#include <random>
#include <utility>

#include <opencv2/core/core.hpp>

inline
int normalizePixel(int value) {
    if (value < 0)
    {
        return 0;
    }
    if (value > 255)
    {
        return 255;
    }
    return value;
}

inline
bool isBlack(cv::Vec3b pix)
{
    return pix[0] == 0 && pix[1] == 0 && pix[2] == 0;
}

inline
bool isWhite(cv::Vec3b pix)
{
    return pix[0] == 255 && pix[1] == 255 && pix[2] == 255;
}

inline
bool areNearlyEqual(const cv::Vec3b &lhs, const cv::Vec3b &rhs, int offset = 5)
{
    return normalizePixel(lhs[0] - offset) <= normalizePixel(rhs[0]) and
           normalizePixel(rhs[0]) <= normalizePixel(lhs[0] + offset) and
           normalizePixel(lhs[1] - offset) <= normalizePixel(lhs[1]) and
           normalizePixel(rhs[1]) <= normalizePixel(lhs[1] + offset) and
           normalizePixel(lhs[2] - offset) <= normalizePixel(rhs[2]) and
           normalizePixel(rhs[2]) <= normalizePixel(lhs[2] + offset);
}

inline
cv::Vec3b getRandomColor()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);

    return {(unsigned char)dist(rng), (unsigned char)dist(rng), (unsigned char)dist(rng)};
}

#endif //POBR_UTILS_H
