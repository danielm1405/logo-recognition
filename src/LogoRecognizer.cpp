//
// Created by dan on 02.01.2020.
//

#include "LogoRecognizer.h"

#include <iostream>

#include "Utils.h"


LogoRecognizer::LogoRecognizer(const std::string &path) {
    original_im_ = cv::imread(path);

    if (original_im_.rows <= 0 or original_im_.cols <= 0)
    {
        throw std::runtime_error("Failed to load file.");
    }

    processed_im_ = original_im_.clone();
}

void LogoRecognizer::smooth() {
    double new_value = 0;

    cv::Mat_<cv::Vec3b> in = processed_im_;
    cv::Mat_<cv::Vec3b> out(in.rows, in.cols, CV_8UC3);

    for (int i = 1; i < in.rows - 1; ++i)
    {
        for (int j = 1; j < in.cols - 1; ++j)
        {
            for (int c = 0; c < in.channels(); ++c)
            {
                out.at<cv::Vec3b>(i, j)[c] = 0;
                new_value = 0;

                for (int k = -1; k < 2; ++k)
                {
                    for (int l = -1; l < 2; ++l)
                    {
                        new_value += (in.at<cv::Vec3b>(i + k, j + l)[c] * (SMOOTHING_KERNEL[k + 1][l + 1]));
                    }
                }

                out.at<cv::Vec3b>(i, j)[c] = normalizePixel(static_cast<int>(new_value));
            }
        }
    }

    processed_im_ = out;
}

void LogoRecognizer::sharpen() {
    double new_value = 0;

    cv::Mat_<cv::Vec3b> in = processed_im_;
    cv::Mat_<cv::Vec3b> out(in.rows, in.cols, CV_8UC3);

    for (int i = 1; i < in.rows - 1; ++i)
    {
        for (int j = 1; j < in.cols - 1; ++j)
        {
            for (int c = 0; c < in.channels(); ++c)
            {
                out.at<cv::Vec3b>(i, j)[c] = 0;
                new_value = 0;

                for (int k = -1; k < 2; ++k)
                {
                    for (int l = -1; l < 2; ++l)
                    {
                        new_value += (in.at<cv::Vec3b>(i + k, j + l)[c] * (SHARPENING_KERNEL[k + 1][l + 1]));
                    }
                }

                out.at<cv::Vec3b>(i, j)[c] = normalizePixel(static_cast<int>(new_value));
            }
        }
    }

    processed_im_ = out;
}

double LogoRecognizer::getMeanSaturation()
{
    double mean_saturation = 0;

    for (int i = 0; i < processed_im_.rows; ++i)
    {
        for (int j = 0; j < processed_im_.cols; ++j)
        {
            mean_saturation += hsv_im_.at<cv::Vec3b>(i, j)[1];
        }
    }

    mean_saturation /= (processed_im_.rows * processed_im_.cols);
    std::cout << "Mean saturation = " << mean_saturation << std::endl;

    return mean_saturation;
}

void LogoRecognizer::inRange(const cv::Scalar &lower, const cv::Scalar &upper)
{
    cv::Mat_<cv::Vec3b> out{hsv_im_.rows, hsv_im_.cols, CV_8UC3};
//    int max = 0;
    for (int i = 0; i < out.rows; ++i)
    {
        for (int j = 0; j < out.cols; ++j)
        {
            bool is_in_range = true;
//            max= std::max(static_cast<int>(hsv_im_.at<cv::Vec3b>(i, j)[2]), max);
            for (int k = 0; k < 3; ++k)
            {
                is_in_range &= hsv_im_.at<cv::Vec3b>(i, j)[k] <= upper[k];
                is_in_range &= hsv_im_.at<cv::Vec3b>(i, j)[k] >= lower[k];
            }

            if (is_in_range)
            {
                out.at<cv::Vec3b>(i, j) = {255, 255, 255};
            }
            else
            {
                out.at<cv::Vec3b>(i, j) = {0, 0, 0};
            }
        }
    }
//    std::cout << max << std::endl;

    thresholded_im_ = std::move(out);
}

Segment LogoRecognizer::floodFill(cv::Mat &I, int i, int j, const cv::Vec3b &new_color)
{
    // I.at<cv::Vec3b>(Y, X) !!!
    std::cout << "floodfilling for " << i << ", " << j  << " with color " << new_color << std::endl;
    auto orig_col = I.at<cv::Vec3b>(i, j);

    std::vector<cv::Point2i> points;
    points.emplace_back(j ,i);
    I.at<cv::Vec3b>(points[0].y, points[0].x) = new_color;

    Segment floodedSegment(new_color);

    for (int k = 0; k < points.size(); ++k)
    {
        if (points[k].x + 1 < I.cols and areNearlyEqual(I.at<cv::Vec3b>(points[k].y, points[k].x + 1), orig_col))
        {
            points.emplace_back((points[k].x) + 1, points[k].y);
            I.at<cv::Vec3b>( points[k].y, points[k].x + 1) = new_color;
        }
        if (points[k].x - 1 > 0 and areNearlyEqual(I.at<cv::Vec3b>( points[k].y, points[k].x - 1), orig_col))
        {
            points.emplace_back(points[k].x - 1, points[k].y);
            I.at<cv::Vec3b>(points[k].y, points[k].x - 1) = new_color;
        }
        if (points[k].y + 1 < I.rows and areNearlyEqual(I.at<cv::Vec3b>(points[k].y + 1, points[k].x), orig_col))
        {
            points.emplace_back(points[k].x, points[k].y + 1);
            I.at<cv::Vec3b>(points[k].y + 1, points[k].x) = new_color;
        }
        if (points[k].y - 1 > 0 and areNearlyEqual(I.at<cv::Vec3b>(points[k].y - 1, points[k].x), orig_col))
        {
            points.emplace_back(points[k].x, points[k].y - 1);
            I.at<cv::Vec3b>(points[k].y - 1, points[k].x) = new_color;
        }
    }

    floodedSegment.pixels_ = std::move(points);
    return floodedSegment;
}

void LogoRecognizer::floodFillAll(const cv::Vec3b &color_to_be_filled, int minimal_object_area)
{
    flood_filled_im_ = thresholded_im_.clone();

    for (int i = 0; i < flood_filled_im_.rows; ++i)
    {
        for (int j = 0; j < flood_filled_im_.cols; ++j)
        {
            if (areNearlyEqual(flood_filled_im_.at<cv::Vec3b>(i, j), color_to_be_filled)) {
                cv::Vec3b new_color = getRandomColor();
                Segment seg = floodFill(flood_filled_im_, i, j, new_color);

                if (seg.pixels_.size() >= minimal_object_area) {
                    segments_.push_back(std::move(seg));
                }
                else
                {
                    // revert flood filling for objects that are not big enough
                    std::cout << "Reverting floodfill because object size (" << seg.pixels_.size() <<
                                 ") < minimal_object_area (" << minimal_object_area << ")." << std::endl;

                    floodFill(flood_filled_im_, i, j, {0, 0, 0});
                }
            }
        }
    }
}

void LogoRecognizer::calculateFeatures()
{
    for (auto &segment : segments_)
    {
        segment.calculateFeatures();
    }
}

void LogoRecognizer::printFeatures() {
    int i = 0;
    for (auto &segment : segments_)
    {
        std::cout << "Features of segment with index " << i << "." << std::endl;
        segment.printAllFeatures();
        ++i;
    }
}

void LogoRecognizer::analyzeFeatures()
{
    for (auto &segment : segments_)
    {
        double inaccuracy = segment.isLetterCWithErrorPercent();

        if (inaccuracy < 0.12)
        {
            std::cout << "Segment qualified as an 'c' with inaccuracy of " << inaccuracy << "." << std::endl;

            cv::circle(flood_filled_im_, {static_cast<int>(segment.x_), static_cast<int>(segment.y_)}, 10,
                    {0, 0, 255}, 3);
        }
    }
}

void LogoRecognizer::showAllAndWait() {
    cv::imshow("Original image", original_im_);
    cv::imshow("Image HSV", hsv_im_);
    cv::imshow("Image thresholded", thresholded_im_);
    cv::imshow("Image floodfilled", flood_filled_im_);

    cv::waitKey(-1);
}