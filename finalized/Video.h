//
// Video class and associated functions
//

#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <fstream>



class Video {

    // underlying data
    cv::VideoCapture capture;
    int cap_width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int cap_height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);

public:

    Video() = default;
    // construct an image from a filename
    Video(const std::string& filename) : capture(cv::VideoCapture(filename)) {
        if (!capture.isOpened()) throw FailedToLoadImgErr{};
    }
    // construct an image from a cv::Mat (cv's image class)
    Video(const cv::VideoCapture cap) : capture(cap) {}

    // display this image, optionally wait for a keystroke to move on
    void show(const std::string& filename = "Video")  {
        cv::Mat frame;
        std::cout << "* Press ESC on the Video Window to exit\n";
        while (capture.read(frame)){
            cv::imshow(filename, frame);
            if (cv::waitKey(1) != -1){
                capture.release();
                std::cout << "finished by user\n";
                break;
            }
            cv::destroyWindow(filename);
        }  
    }
    
//implement save     
    Video saveAs(const std::string& filename="save.avi"){
        std::cout << "Saving Video..." << "\n";
        cv::VideoWriter output(filename, cv::VideoWriter::fourcc('M','J','P','G'), 0, cv::Size(cap_width,cap_height));
        cv::Mat frame;
        while(capture.read(frame)){
            output.write(frame);
        }
        output.release();
        return Video(filename);
    }

    Video grayscale(){
        cv::VideoWriter output("grayscale.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(cap_width,cap_height));
        cv::Mat frame;
        std::cout << "Saving Grayscale Video..." << "\n";
        while(capture.read(frame)){
            cv::Mat ret;
            cv::cvtColor(frame, ret, cv::COLOR_BGR2GRAY);
            cv::imshow("Grayscale", ret);
            if (cv::waitKey(1) != -1){
                capture.release();
                std::cout << "finished by user\n";
                break;
            }
            output.write(ret);
        }
        output.release();
        cv::destroyWindow("Grayscale");
        return Video("grayscale.avi"); 
    }
        


    Video edge_detect(const int lower_threshold, const int upper_threshold)  {
        cv::VideoWriter output("edge_detection_video.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(cap_width,cap_height));
        cv::Mat frame;
        std::cout << "Saving Edge Detection Video..." << "\n";
        while(capture.read(frame)){
            cv::Mat ret;
            cv::Canny(frame, ret, lower_threshold, upper_threshold);
            cv::imshow("Edge Detection", ret);
            if (cv::waitKey(1) != -1){
                capture.release();
                std::cout << "finished by user\n";
                break;
            }
            output.write(ret);
        }
        output.release();
        cv::destroyWindow("Edge Detection");
        return Video("edge_detection_video.avi"); 
    }

    Video gaussian_blur(const int kernel_sz)  {
        debug_assert(kernel_sz % 2, "Kernel size must be an odd number");
        debug_assert(kernel_sz > 1, "Kernel size must be greater than 1");
        debug_assert(kernel_sz < 1000, "Kernel size must be less than 1000");

        cv::VideoWriter output("gaussian_blur.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(cap_width,cap_height));
        cv::Mat frame;
        std::cout << "Saving Blurred Video..." << "\n";
        while(capture.read(frame)){
            cv::Mat ret;
            cv::GaussianBlur(frame, ret, cv::Size(kernel_sz, kernel_sz), 0);
            cv::imshow("Gaussisan Blurring", ret);
            if (cv::waitKey(1) != -1){
                capture.release();
                std::cout << "finished by user\n";
                break;
            }
            output.write(ret);
        }
        cv::destroyWindow("Gaussian Blurring");
        output.release();
        return Video("gaussian_blur.avi"); 
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

    std::vector<cv::Point> collect_points(const std::string& window_name = "_tmp_collect")  {
        cv::Mat img; 
        capture.read(img);
        std::vector<cv::Point> ret;

        cv::namedWindow(window_name, 1);
        cv::setMouseCallback(window_name, Video::add_point_cb, reinterpret_cast<void*>(&ret));
        cv::imshow(window_name, img);
        cv::waitKey(0);
        cv::destroyWindow(window_name);

        if (ret.size() < 3) throw NotEnoughPointsErr{};

        return ret;
    }

    Video create_homography(const std::vector<cv::Point>& points) {
        debug_assert(points.size() == 4, "Exactly 4 points must be given");

        const std::vector<cv::Point> dst_points = {
            cv::Point(0,        0),
            cv::Point(cap_width, 0),
            cv::Point(cap_width, cap_height),
            cv::Point(0         , cap_height),
        };
        cv::Mat h = cv::findHomography(points, dst_points);

        cv::VideoWriter output("create_homography.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(cap_width,cap_height));
        cv::Mat frame;
        std::cout << "Saving Perspective Shifted Video..." << "\n";
        while(capture.read(frame)){
            cv::Mat ret;
            cv::warpPerspective(frame, ret, h, frame.size());
            cv::imshow("Shifting Perspective", ret);
            if (cv::waitKey(1) != -1){
                capture.release();
                output.release();
                std::cout << "finished by user\n";
                break;
            }
            cv::destroyWindow("Shifting Perspective");
            output.write(ret);
        }
        output.release();
        return Video("create_homography.avi"); 

    }

    Video threshold(const int type, const int value) {
        debug_assert(type >= 1, "Threshold type must be at least 1");
        debug_assert(type <= 5, "Threshold type must be at most 5");
        debug_assert(value >= 0, "Threshold value must be non-negative");
        debug_assert(value < 256, "Threshold value must be less than 256");

        cv::VideoWriter output("threshold.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(cap_width,cap_height));
        cv::Mat frame;
        std::cout << "Saving Thresholded Video..." << "\n";
        while(capture.read(frame)){
            cv::Mat gray_frame;
            cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
            cv::Mat ret;
            cv::threshold(gray_frame, ret, value, 255, type);
            cv::imshow("Video Thresholding", ret);
            if (cv::waitKey(1) != -1){
                capture.release();
                output.release();
                std::cout << "finished by user\n";
                break;
            }
            output.write(ret);
        }
        cv::destroyWindow("Video Thresholding");
        output.release();
        return Video("threshold.avi"); 

    } 

    Video track(){
        cv::Ptr<cv::TrackerKCF> tracker = cv::TrackerKCF::create();

        cv::Mat frame;
        capture.read(frame);

        cv::Rect box;
        box = cv::selectROI(frame, false);
        cv::rectangle(frame, box, cv::Scalar(255, 0, 0), 2, 1);
        imshow("Tracker Frame 1", frame);
        cv::destroyWindow("Tracker Frame 1");
        tracker->init(frame, box);

        cv::VideoWriter output("tracker.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(cap_width,cap_height));
        auto start = std::chrono::high_resolution_clock::now();
        int frame_count = 0;
        int total_frames = 0;
        float fps = -1;
        std::cout << "Saving Tracked Video..." << "\n";
        while(capture.read(frame)){

            frame_count++;
            total_frames++;
            
            bool updated = tracker->update(frame, box);
            if (updated){
                rectangle(frame, box, cv::Scalar(255,0,0),2,1);
            }
            else{
                putText(frame, "Error: object tracking failed", cv::Point(100, 80), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0,0,255),2);
            }

            if (frame_count >= 30){
                auto end = std::chrono::high_resolution_clock::now();
                fps = frame_count * 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                frame_count = 0;
                start = std::chrono::high_resolution_clock::now();
            }
            
            putText(frame, "FPS: " + std::to_string(fps) , cv::Point(100, 50), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0,255,0),2);
            imshow("Tracking", frame);

            if (cv::waitKey(1) != -1){
                capture.release();
                output.release();
                std::cout << "finished by user\n";
                break;
            }
            output.write(frame);
        }
         cv::destroyWindow("Tracking");
        output.release();
        return Video("tracker.avi"); 

    }

private:

    struct Detection{
        int labelId;
        float confidence;
        cv::Rect box;
    };

    std::vector<std::string> loadClassifications(){
        std::vector<std::string> classes;
        std::ifstream ifs("model/classes.txt");
        std::string line;
        while (getline(ifs, line)){
            classes.push_back(line);
        }    
        return classes;
    }

    cv::dnn::Net loadDNN(){
        cv::dnn::Net nn; nn = cv::dnn::readNet("model/yolov5s.onnx");
        nn.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        nn.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        return nn;
    }

    cv::Mat formatInput(cv::Mat &image){
        int row = image.rows; 
        int col = image.cols;
        int sides = MAX(col, row);
        cv::Mat input; input = cv::Mat::zeros(sides, sides, CV_8UC3);
        image.copyTo(input(cv::Rect(0,0,col,row)));
        return input;
    }

    std::vector<Detection> obtainOutput(cv::dnn::Net &net, cv::Mat &input, const std::vector<std::string> labels){
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

    void putDetection(std::vector<Detection> &output, cv::Mat &frame, const std::vector<std::string> labels){
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
    Video detection(){
        std::vector<std::string> label = Video::loadClassifications();
        cv::dnn::Net net = Video::loadDNN();

        cv::VideoWriter output("detect.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(cap_width,cap_height));
        auto start = std::chrono::high_resolution_clock::now();
        int frame_count = 0;
        int total_frames = 0;
        float fps = -1;
        
        cv::Mat frame;
        capture.read(frame);
        std::cout << "Saving Detected Video..." << "\n";
        while(capture.read(frame)){

            frame_count++;
            total_frames++;
            
            cv::Mat input; 
            input = Video::formatInput(frame);

            std::vector<Detection> detections; 
            detections = Video::obtainOutput(net, input, label);

            Video::putDetection(detections, frame, label); 

            imshow("Detection", frame);

            if (cv::waitKey(1) != -1){
                output.release();
                capture.release();
                std::cout << "finished by user\n";
                break;
            }
            output.write(frame);
        }
        cv::destroyWindow("Detection");
        output.release();
        return Video("detect.avi"); 

    }
  

};