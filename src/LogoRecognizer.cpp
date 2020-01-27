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

void LogoRecognizer::run(bool verbose)
{
    // quality improvement
    sharpen();
    smooth();
    if (verbose)
    {
        showAllAndWait();
    }

    // color space conversion
    cv::cvtColor(processed_im_, processed_im_, cv::COLOR_BGR2HSV);
    if (verbose)
    {
        showAllAndWait();
    }

    // adaptive thresholding
    auto mean_saturation = getMeanSaturation();
    cv::Scalar lower_limit{100,  mean_saturation, 40};
    cv::Scalar upper_limit{130, 255, 180};
    inRange(lower_limit, upper_limit);
    if (verbose)
    {
        showAllAndWait();
    }

    // segmentation
    cv::Vec3b color_to_fill{255, 255, 255};
    int minimal_object_area{80};
    floodFillAll(color_to_fill, minimal_object_area);
    if (verbose)
    {
        showAllAndWait();
    }

    // features calculation
    calculateFeatures();

    // features analysis
    analyzeFeatures();
    if (verbose)
    {
        showAllAndWait();
    }

    // decide where are logos
    findLogos();

    // images display
    showAllAndWait();
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

    processed_im_ = std::move(out);
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

    processed_im_ = std::move(out);
}

double LogoRecognizer::getMeanSaturation()
{
    double mean_saturation = 0;

    for (int i = 0; i < processed_im_.rows; ++i)
    {
        for (int j = 0; j < processed_im_.cols; ++j)
        {
            mean_saturation += processed_im_.at<cv::Vec3b>(i, j)[1];
        }
    }

    mean_saturation /= (processed_im_.rows * processed_im_.cols);
    std::cout << "Mean saturation = " << mean_saturation << std::endl;

    return mean_saturation;
}

void LogoRecognizer::inRange(const cv::Scalar &lower, const cv::Scalar &upper)
{
    cv::Mat_<cv::Vec3b> out{processed_im_.rows, processed_im_.cols, CV_8UC3};

    for (int i = 0; i < out.rows; ++i)
    {
        for (int j = 0; j < out.cols; ++j)
        {
            bool is_in_range = true;

            for (int k = 0; k < 3; ++k)
            {
                is_in_range &= processed_im_.at<cv::Vec3b>(i, j)[k] <= upper[k];
                is_in_range &= processed_im_.at<cv::Vec3b>(i, j)[k] >= lower[k];
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

    processed_im_ = std::move(out);
}

Segment LogoRecognizer::floodFill(cv::Mat &I, int i, int j, const cv::Vec3b &new_color)
{
    auto orig_col = I.at<cv::Vec3b>(i, j);

    std::vector<cv::Point2i> points;
    points.emplace_back(j ,i);
    I.at<cv::Vec3b>(points[0].y, points[0].x) = new_color;

    Segment floodedSegment(new_color);

    for (int k = 0; k < points.size(); ++k)
    {
        if (points[k].x + 1 < I.cols and I.at<cv::Vec3b>(points[k].y, points[k].x + 1) == orig_col)
        {
            points.emplace_back((points[k].x) + 1, points[k].y);
            I.at<cv::Vec3b>( points[k].y, points[k].x + 1) = new_color;
        }
        if (points[k].x - 1 > 0 and I.at<cv::Vec3b>( points[k].y, points[k].x - 1) == orig_col)
        {
            points.emplace_back(points[k].x - 1, points[k].y);
            I.at<cv::Vec3b>(points[k].y, points[k].x - 1) = new_color;
        }
        if (points[k].y + 1 < I.rows and I.at<cv::Vec3b>(points[k].y + 1, points[k].x) == orig_col)
        {
            points.emplace_back(points[k].x, points[k].y + 1);
            I.at<cv::Vec3b>(points[k].y + 1, points[k].x) = new_color;
        }
        if (points[k].y - 1 > 0 and I.at<cv::Vec3b>(points[k].y - 1, points[k].x) == orig_col)
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
    auto out = processed_im_.clone();

    for (int i = 0; i < out.rows; ++i)
    {
        for (int j = 0; j < out.cols; ++j)
        {
            if (out.at<cv::Vec3b>(i, j) == color_to_be_filled)
            {
                cv::Vec3b new_color = getRandomColor();
                Segment seg = floodFill(out, i, j, new_color);

                if (seg.pixels_.size() >= minimal_object_area)
                {
                    segments_.push_back(std::move(seg));
                }
                else
                {
                    floodFill(out, i, j, {0, 0, 0});
                }
            }
        }
    }

    processed_im_ = std::move(out);
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
        if (segment.isLetterC())
        {
            std::cout << "Segment qualified as an 'c'." << std::endl;
            segment.printAllFeatures();

            cv::circle(processed_im_, {static_cast<int>(segment.x_), static_cast<int>(segment.y_)}, 10,
                    {0, 0, 255}, 1);

            c_letters_.push_back(&segment);
        }

        if (segment.isLetterI())
        {
            std::cout << "Segment qualified as an 'i'." << std::endl;
            segment.printAllFeatures();

            cv::circle(processed_im_, {static_cast<int>(segment.x_), static_cast<int>(segment.y_)}, 10,
                       {0, 255, 255}, 1);

            i_letters_.push_back(&segment);
        }

        if (segment.isLetterT())
        {
            std::cout << "Segment qualified as an 't'." << std::endl;
            segment.printAllFeatures();

            cv::circle(processed_im_, {static_cast<int>(segment.x_), static_cast<int>(segment.y_)}, 10,
                       {255, 0, 255}, 1);

            t_letters_.push_back(&segment);
        }
    }

    std::cout << "Found " << c_letters_.size() << " c's, " <<
              i_letters_.size() << " i's, " <<
              t_letters_.size() << " t's." << std::endl;
}

void LogoRecognizer::findLogos()
{
    for (auto & c_letter : c_letters_)
    {
        for (auto & i_letter : i_letters_)
        {
            if (areConsecutiveLetters(*c_letter, *i_letter))
            {
                std::cout << "CI found..." << std::endl;

                for (auto & t_letter : t_letters_)
                {
                    if (areConsecutiveLetters(*i_letter, *t_letter))
                    {
                        std::cout << "CIT found..." << std::endl;

                        for (auto & i_letter2 : i_letters_)
                        {
                            if (areConsecutiveLetters(*t_letter, *i_letter2))
                            {
                                std::cout << "CITI found!!!" << std::endl;

                                std::vector<Segment> points{*c_letter, *i_letter, *t_letter, *i_letter2};
                                auto box = getBox(points);

                                cv::rectangle(processed_im_, box.first, box.second,
                                        {255, 0, 255}, 3);
                            }
                        }
                    }
                }
            }
        }
    }
}

void LogoRecognizer::showAllAndWait() {
    cv::imshow("Original image", original_im_);
    cv::imshow("Processed Image", processed_im_);

    cv::waitKey(-1);
}