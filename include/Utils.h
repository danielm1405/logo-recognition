//
// Created by dan on 02.01.2020.
//

#ifndef POBR_UTILS_H
#define POBR_UTILS_H

#include <random>
#include <utility>


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
cv::Vec3b getRandomColor()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);

    return {(unsigned char)dist(rng), (unsigned char)dist(rng), (unsigned char)dist(rng)};
}

#endif //POBR_UTILS_H
