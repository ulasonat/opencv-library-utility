//
// Image class and associated functions
//

#pragma once

#include <opencv2/opencv.hpp>

#include <string>
#include <vector>
#include <cassert>


struct NotEnoughPointsErr {};

class Image {

    // underlying data
    cv::Mat img;

public:

    Image() = default;
    // construct an image from a filename
    Image(const std::string& filename) : img(cv::imread(filename)) {}
    // construct an image from a cv::Mat (cv's image class)
    Image(const cv::Mat _img) : img(_img) {}

    // display this image, optionally wait for a keystroke to move on
    void show(const std::string& filename = "", bool wait_key = true) const {
        cv::imshow(filename, img);

        if (wait_key)
            cv::waitKey(0);
    }

    // clears all currently displayed windows
    static void destroy_all_windows() { cv::destroyAllWindows(); }

    // resize this image such that it matches the size of other
    void fit_to_size(const Image& other) { cv::resize(img, img, other.img.size()); }

    // returns the result of blending this image with another image at given weight
    Image alpha_blend(const Image& other, const double other_weight) const {
        Image ret;
        cv::addWeighted(img, (1 - other_weight), other.img, other_weight, ret.img);
        return ret;
    }

    // returns an image with pixels hot on edges and cold elsewhere
    Image edge_detect(const int lower_threshold, const int upper_threshold) const {
        Image ret;
        cv::Canny(img, ret.img, lower_threshold, upper_threshold);
        return ret;
    }

    // returns the result of blurring this image
    Image guassian_blur(const int kernel_sz) const {
        assert(kernel_sz % 2, "Kernel size must be an odd number");
        assert(kernel_sz > 1, "Kernel size must be greater than 1");
        assert(kernel_sz < 1000, "Kernel size must be less than 1000");

        Image ret;
        cv::GuassianBlur(img, ret.img, cv::Size(kernel_sz, kernel_sz), 0);
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
    std::vector<cv::Point> collect_points(const int nr_points, const std::string& window_name = "") const {
        std::vector<cv::Point> ret;

        cv::namedWindow(window_name, 1);
        cv::setMouseCallback(window_name, Image::add_point_cb, reinterpret_cast<void*>(&ret));
        cv::imshow(window_name, img);
        cv::waitKey(0);

        if (ret.size() < nr_points) throw NotEnoughPointsErr{};

        ret.resize(nr_points);
        return ret;
    }

    // returns the homography of a subimage of this image, needs 4 points to define subimage
    Image create_homography(const std::vector<cv::Point>& points) const {
        assert(points.size() == 4, "Exactly 4 points must be given");

        const std::vector<cv::Point> dst_points = {
            cv::Point(0,        0),
            cv::Point(img.cols, 0),
            cv::Point(0,        img.rows),
            cv::Point(img.cols, img.rows),
        };

        cv::Mat tmp = cv::findHomography(points, dst_points);
        Image ret;
        cv::warpPerspective(img, ret.img, tmp, cv::size(img));

        return ret;
    }

    // returns the mean of these points
    // TODO @vicente: is this necessary?
    static cv::Point get_polygon_mean(const std::vector<cv::Point>& points) const {
        int sum_x = 0, sum_y = 0;

        for (const cv::Point& point : points) {
            sum_x += point.x;
            sum_y += point.y;
        }

        return cv::Point(sum_x / points.size(), sum_y / points.size());
    }

    // TODO @nick, @vicente is proj_img.cpp done?

    // return the grayscale version of this image
    Image grayscale() const {
        Image ret;
        cvtColor(img, ret.img, COLOR_BGR2GRAY);
        return ret;
    }

    // threshold this image with one of the given types using some threshold value
    Image threshold(const int type, const int value) const {
        assert(type >= 1, "Threshold type must be at least 1");
        assert(type <= 5, "Threshold type must be at most 5");
        assert(value >= 0, "Threshold value must be non-negative");
        assert(value < 256, "Threshold value must be less than 256");

        Image ret;
        cv::threshold(grayscale().img, ret.img, value, 255, type);
        return ret;
    }
}
