//
// Image class and associated functions
//

#pragma once

#include <opencv2/opencv.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <fstream>


struct NotEnoughPointsErr {};
struct FailedToLoadImgErr {};


static void debug_assert(bool var, const std::string& msg = "") {
    if (!var)
        std::cerr << msg << std::endl;

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
    void show(const std::string& filename = "Image") const {
        cv::namedWindow(filename, 1);
        cv::imshow(filename, img);
        std::cout << "* Press 0 on the Image Window to continue\n";
        cv::waitKey(0);
        cv::destroyWindow(filename);
    }

    // save an image to the current directory
    void save(const std::string filename) {
        cv::imwrite(filename, img);
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
        if (event == cv::EVENT_LBUTTONDOWN){
            reinterpret_cast<std::vector<cv::Point>*>(data)->push_back(cv::Point(x, y));
            std::cout << "Selected point: (" << x << "," << y << ")\n";
        }
    }

public:

    // displays image, reads nr_points points, then waits for 0 to close
    std::vector<cv::Point> collect_points(const std::string& window_name = "_tmp_collect") const {
        std::vector<cv::Point> ret;

        cv::namedWindow(window_name, 1);
        cv::setMouseCallback(window_name, Image::add_point_cb, reinterpret_cast<void*>(&ret));
        cv::imshow(window_name, img);
        cv::waitKey(0);
        cv::destroyWindow(window_name);

        if (ret.size() < 3) throw NotEnoughPointsErr{};

        return ret;
    }

    // returns the homography of a subimage of this image, needs 4 points to define subimage
    Image create_homography(const std::vector<cv::Point>& points) const {
        debug_assert(points.size() == 4, "Exactly 4 points must be given");

        const std::vector<cv::Point> dst_points = {
            cv::Point(0,        0),
            cv::Point(img.cols, 0),
            cv::Point(img.rows  , img.rows),
            cv::Point(0         , img.rows),
        };

        cv::Mat tmp = cv::findHomography(points, dst_points);
        Image ret;
        cv::warpPerspective(img, ret.img, tmp, img.size());

        return ret;
    }

    int get_0th_moment(const std::vector<cv::Point>& points) const{
        cv::Mat mask(img.rows, img.cols, CV_8UC3, cv::Scalar(0,0,0));
        int sz = points.size();
        cv::Point pnts[sz];
        for(int i=0; i<sz; i++){
            pnts[i] = points[i];
        }
        cv::fillConvexPoly(mask, pnts, sz, cv::Scalar(255, 255, 255));
        imshow("Mask", mask);
        cv::Mat gray; cv::cvtColor(mask, gray, cv::COLOR_BGR2GRAY);
        int Area = cv::countNonZero(gray);
        return Area;
    }

    // returns the mean of these points
    static cv::Point get_1st_moment(const std::vector<cv::Point>& points) {
        int sum_x = 0, sum_y = 0;

        for (const cv::Point& point : points) {
            sum_x += point.x;
            sum_y += point.y;
        }

        return cv::Point(sum_x / points.size(), sum_y / points.size());
    }

    Image get_mask(const std::vector<cv::Point>& points) const {

        //create mask
        int sz = points.size();
        cv::Point pnts[sz];
        for(int i=0; i<sz; i++){
            pnts[i] = points[i];
        }
        cv::Mat mask(img.rows, img.cols, CV_8UC3, cv::Scalar(0,0,0));
        cv::fillConvexPoly(mask, pnts, sz, cv::Scalar(255, 255, 255));

        //combine mask and portion of image
        cv::Mat ret; cv::bitwise_and(mask, img, ret);

        return Image(ret);
    }

    // return the result of projecting specified portion of other
    // onto specified portion of this
    Image proj_img(
        const Image& other,
        const std::vector<cv::Point>& other_points,
        const std::vector<cv::Point>& this_points) const {
        debug_assert(this_points.size() == 4, "Exactly 4 points must be given");
        debug_assert(other_points.size() == 4, "Exactly 4 points must be given");
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
        cvtColor(ret.img, ret.img, cv::COLOR_GRAY2RGB);
        return ret;
    }

    // threshold this image with one of the given types using some threshold value
    Image threshold(const int type, const int value) const {
        debug_assert(type >= 1, "Threshold type must be at least 1");
        debug_assert(type <= 5, "Threshold type must be at most 5");
        debug_assert(value >= 0, "Threshold value must be non-negative");
        debug_assert(value < 256, "Threshold value must be less than 256");

        Image ret;
        cv:: threshold(img ,ret.img, value, 255, type );
        return ret;
    }


// ------------------------- 1.0 Additional Implementation
private:

    struct Detection{
        int labelId;
        float confidence;
        cv::Rect box;
    };

    std::vector<std::string> loadClassifications() const{
        std::vector<std::string> classes;
        std::ifstream ifs("model/classes.txt");
        std::string line;
        while (getline(ifs, line)){
            classes.push_back(line);
        }    
        return classes;
    }

    cv::dnn::Net loadDNN() const{
        cv::dnn::Net nn; nn = cv::dnn::readNet("model/yolov5s.onnx");
        nn.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        nn.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        return nn;
    }

    cv::Mat formatInput(cv::Mat &image) const{
        int row = image.rows; 
        int col = image.cols;
        int sides = MAX(col, row);
        cv::Mat input; input = cv::Mat::zeros(sides, sides, CV_8UC3);
        image.copyTo(input(cv::Rect(0,0,col,row)));
        return input;
    }

    std::vector<Detection> obtainOutput(cv::dnn::Net &net, cv::Mat &input, const std::vector<std::string> labels) const{
        const float INPUT_SIDES = 640.0;
        const float SCORE_THRESHOLD = 0.2;
        const float NMS_THRESHOLD = 0.4;
        const float CONFIDENCE_THRESHOLD = 0.4;

        cv::Mat blob;
        cv::dnn::blobFromImage(input, blob, 1./255., cv::Size(INPUT_SIDES, INPUT_SIDES), cv::Scalar(), true, false);
        net.setInput(blob);

        std::vector<cv::Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());
        
        // Scaling factor from input image to blob input
        float x_factor = input.cols / INPUT_SIDES;
        float y_factor = input.rows / INPUT_SIDES;
        // extract detection data
        float *data = (float *)outputs[0].data;

        std::vector<int> labelIds;
        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;
        const int ROWS = 25200;
        for (int i = 0; i < ROWS; ++i) {

            float confidence = data[4];
            if (confidence >= CONFIDENCE_THRESHOLD) {

                float * label_scores = data + 5;
                cv::Mat scores(1, labels.size(), CV_32FC1, label_scores);
                cv::Point labelId;
                double max_label_score;
                minMaxLoc(scores, 0, &max_label_score, 0, &labelId);
                if (max_label_score > SCORE_THRESHOLD) {
                    float x = data[0];
                    float y = data[1];
                    float w = data[2];
                    float h = data[3];
                    int left = int((x - 0.5 * w) * x_factor);
                    int top = int((y - 0.5 * h) * y_factor);
                    int width = int(w * x_factor);
                    int height = int(h * y_factor);

                    boxes.push_back(cv::Rect(left, top, width, height));
                    confidences.push_back(confidence);
                    labelIds.push_back(labelId.x);
                }
            }
            data += 85;
        }

        // suppress overlapping boxes/detections with Non-maximal supression
        std::vector<Detection> output;
        std::vector<int> nms_result;
        cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nms_result);
        for (int i = 0; i < nms_result.size(); i++) {
            int idx = nms_result[i];
            Detection result;
            result.labelId = labelIds[idx];
            result.confidence = confidences[idx];
            result.box = boxes[idx];
            output.push_back(result);
        }
        return output;
    }

    void putDetection(std::vector<Detection> &output, cv::Mat &frame, const std::vector<std::string> labels) const{
        const std::vector<cv::Scalar> colors = {cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255), cv::Scalar(255, 0, 0)};
        for (int i = 0; i < output.size(); ++i){
                auto detection = output[i];
                auto box = detection.box;
                auto labelId = detection.labelId;
                const auto color = colors[labelId % colors.size()];
                cv::rectangle(frame, box, color, 3);
                cv::rectangle(frame, cv::Point(box.x, box.y - 20), cv::Point(box.x + box.width, box.y), color, cv::FILLED);
                cv::putText(frame, labels[labelId].c_str(), cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
            }
    }

public:
    Image detection() const{
        cv::Mat ret = img;
        std::vector<std::string> label = Image::loadClassifications();
        cv::dnn::Net net = Image::loadDNN();
        cv::Mat input; input = Image::formatInput(ret);
        std::vector<Detection> output; output = Image::obtainOutput(net, input, label);
        Image::putDetection(output, ret, label); 
        return Image(ret);
    }

};