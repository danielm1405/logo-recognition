//
// Created by dan on 03.01.2020.
//

#include "Segment.h"

void Segment::calculateFeatures()
{
    for (const auto &point : pixels_)
    {
        int i = point.x;
        int j = point.y;

        m00_++;
        m10_ += i;
        m01_ += j;
        m11_ += i * j;
        m20_ += i * i;
        m02_ += j * j;
        m21_ += i * i * j;
        m12_ += i * j * j ;
        m30_ += i * i * i;
        m03_ += j * j * j;
    }

    x_ = m10_ / m00_;
    y_ = m01_ / m00_;

    M00_ = m00_;
    M11_ = m11_ - (m10_ * m01_ / m00_);
    M20_ = m20_ - (m10_ * m10_ / m00_);
    M02_ = m02_ - (m01_ * m01_ / m00_);
    M21_ = m21_ - 2 * m11_ * x_ - m20_ * y_ + 2 * m01_ * x_ * x_;
    M12_ = m12_ - 2 * m11_ * y_ - m02_ * x_ + 2 * m10_ * y_ * y_;
    M30_ = m30_ - 3 * m20_ * x_ + 2 * m10_ * x_ * x_;
    M03_ = m03_ - 3 * m02_ * y_ + 2 * m01_ * y_ * y_;

    M1_ = (M20_ + M02_) / std::pow(M00_, 2);
    M2_ = (std::pow(M20_ - M02_, 2) + 4 * (std::pow(M11_, 2))) / std::pow(M00_, 4);
    M3_ = (std::pow(M30_ - 3 * M12_, 2) + std::pow(3 * M21_ - M03_, 2)) / std::pow(M00_, 5);
    M4_ = (std::pow(M30_ + M12_, 2) + std::pow(M21_ + M03_, 2)) / std::pow(M00_, 5);
    M7_ = (M20_ * M02_ - std::pow(M11_, 2)) / std::pow(M00_, 4);
}

bool Segment::isLetterC()
{
    return M1_ > 0.35 and M1_ < 0.53 and
           M2_ < 0.05 and
           M3_ > 0.001 and M3_ < 0.03 and
           M4_ > 0.001 and M4_ < 0.02 and
           M7_ > 0.02 and M7_ < 0.07;
}

bool Segment::isLetterI()
{
    return M1_ > 0.28 and M1_ < 0.7 and
           M2_ > 0.06 and M2_ < 0.32 and
           M3_ < 0.002 and
           M4_ < 0.002 and
           M7_ > 0.001 and M7_ < 0.01;
}

bool Segment::isLetterT()
{
    return M1_ > 0.3 and M1_ < 0.7 and
           M2_ > 0.05 and M2_ < 0.35 and
           M3_ > 0.002 and M3_ < 0.05 and
           M4_ > 0.0005 and M4_ < 0.03 and
           M7_ > 0.01 and M7_ < 0.019;
}

void Segment::printAllFeatures()
{
//    std::cout << "\tm00: " << m00_ << std::endl;
//    std::cout << "\tm10: " << m10_ << std::endl;
//    std::cout << "\tm01: " << m01_ << std::endl;
//    std::cout << "\tm11: " << m11_ << std::endl;
//    std::cout << "\tm20: " << m20_ << std::endl;
//    std::cout << "\tm02: " << m02_ << std::endl;
    std::cout << "\tM1: " << M1_ << std::endl;
    std::cout << "\tM2: " << M2_ << std::endl;
    std::cout << "\tM3: " << M3_ << std::endl;
    std::cout << "\tM4: " << M4_ << std::endl;
    std::cout << "\tM7: " << M7_ << std::endl;
}
