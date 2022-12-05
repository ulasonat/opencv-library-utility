//
// Image class and associated functions
//

#pragma once

#include <opencv2/opencv.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cassert>


struct NotEnoughPointsErr {};
struct FailedToLoadImgErr {};

static void debug_assert(bool var, const std::string& msg = "") {
    if (!var)
        std::cerr << msg << "\n";

    assert(var);
}

class Image {

    // underlying data
    cv::Mat img;

public:

    Image() = default;
    // construct an image from a filename
    Image(const std::string& filename) : img(cv::imread(filename)) {
        if (img.empty()) throw FailedToLoadImgErr{};
    }
    // construct an image from a cv::Mat (cv's image class)
    Image(const cv::Mat _img) : img(_img) {}

    // display this image, optionally wait for a keystroke to move on
    void show(const std::string& filename = "_tmp_show") const {
        cv::namedWindow(filename, 1);
        cv::imshow(filename, img);
        std::cout << "* Press 0 to continue\n";
        cv::waitKey(0);
        cv::destroyWindow(filename);
    }

    // clears all currently displayed windows
    static void destroy_all_windows() { cv::destroyAllWindows(); }

    // resize this image such that it matches the size of other
    void fit_to_size(const Image& other) { cv::resize(img, img, other.img.size()); }

    // returns the result of blending this image with another image at given weight
    Image alpha_blend(const Image& other, const double other_weight) const {
        Image ret;
        cv::addWeighted(img, (1 - other_weight), other.img, other_weight, 0.0, ret.img);
        return ret;
    }

    // returns an image with pixels hot on edges and cold elsewhere
    Image edge_detect(const int lower_threshold, const int upper_threshold) const {
        Image ret;
        cv::Canny(img, ret.img, lower_threshold, upper_threshold);
        return ret;
    }

    // returns the result of blurring this image
    Image gaussian_blur(const int kernel_sz) const {
        debug_assert(kernel_sz % 2, "Kernel size must be an odd number");
        debug_assert(kernel_sz > 1, "Kernel size must be greater than 1");
        debug_assert(kernel_sz < 1000, "Kernel size must be less than 1000");

        Image ret;
        cv::GaussianBlur(img, ret.img, cv::Size(kernel_sz, kernel_sz), 0);
        return ret;
    }

private:

    // callback for mouse clicking, adds this point to the vector
    static void add_point_cb(int event, int x, int y, int flags, void* data) {
        if (event == cv::EVENT_LBUTTONDOWN)
            reinterpret_cast<std::vector<cv::Point>*>(data)->push_back(cv::Point(x, y));
    }

public:

    // displays image, reads nr_points points, then waits for 0 to close
    std::vector<cv::Point> collect_points(const int nr_points, const std::string& window_name = "_tmp_collect") const {
        std::vector<cv::Point> ret;

        cv::namedWindow(window_name, 1);
        cv::setMouseCallback(window_name, Image::add_point_cb, reinterpret_cast<void*>(&ret));
        cv::imshow(window_name, img);
        cv::waitKey(0);
        cv::destroyWindow(window_name);

        if (ret.size() < nr_points) throw NotEnoughPointsErr{};

        ret.resize(nr_points);
        return ret;
    }

    // returns the homography of a subimage of this image, needs 4 points to define subimage
    Image create_homography(const std::vector<cv::Point>& points) const {
        debug_assert(points.size() == 4, "Exactly 4 points must be given");

        const std::vector<cv::Point> dst_points = {
            cv::Point(0,        0),
            cv::Point(img.cols, 0),
            cv::Point(0,        img.rows),
            cv::Point(img.cols, img.rows),
        };

        cv::Mat tmp = cv::findHomography(points, dst_points);
        Image ret;
        cv::warpPerspective(img, ret.img, tmp, img.size());

        return ret;
    }

    // returns the mean of these points
    static cv::Point get_polygon_mean(const std::vector<cv::Point>& points) {
        int sum_x = 0, sum_y = 0;

        for (const cv::Point& point : points) {
            sum_x += point.x;
            sum_y += point.y;
        }

        return cv::Point(sum_x / points.size(), sum_y / points.size());
    }

    // return the result of projecting specified portion of other
    // onto specified portion of this
    Image proj_img(
        const Image& other,
        const std::vector<cv::Point>& other_points,
        const std::vector<cv::Point>& this_points) const {

        Image ret(img);

        // mask out the projection site on dst
        cv::fillConvexPoly(ret.img, this_points.data(), 4, cv::Scalar(0, 0, 0));

        // mask out the rest of src
        cv::Mat src(other.img.rows, other.img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        cv::fillConvexPoly(src, other_points.data(), 4, cv::Scalar(255, 255, 255));
        cv::bitwise_and(src, other.img, src);

        // project src cutout onto dst cutout
        cv::warpPerspective(
            src,
            src,
            cv::findHomography(other_points, this_points),
            img.size()
        );

        // combine the cutouts into the output
        cv::bitwise_or(ret.img, src, ret.img);

        return ret;
    }

    // return the grayscale version of this image
    Image grayscale() const {
        Image ret;
        cv::cvtColor(img, ret.img, cv::COLOR_BGR2GRAY);
        return ret;
    }

    // threshold this image with one of the given types using some threshold value
    Image threshold(const int type, const int value) const {
        debug_assert(type >= 1, "Threshold type must be at least 1");
        debug_assert(type <= 5, "Threshold type must be at most 5");
        debug_assert(value >= 0, "Threshold value must be non-negative");
        debug_assert(value < 256, "Threshold value must be less than 256");

        Image ret;
        cv::threshold(grayscale().img, ret.img, value, 255, type);
        return ret;
    }
};
