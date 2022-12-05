#include <fstream>
#include <opencv2/opencv.hpp>

const std::vector<cv::Scalar> colors = {cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255), cv::Scalar(255, 0, 0)};
const float INPUT_SIDES = 640.0;

// maybe adjust model with label score, nms_threshold, confidence_threshold ???
const float SCORE_THRESHOLD = 0.2;
const float NMS_THRESHOLD = 0.4;
const float CONFIDENCE_THRESHOLD = 0.4;

struct Detection{
    int labelId;
    float confidence;
    cv::Rect box;
};

int main(){

    // read in label list text file
    std::vector<std::string> labels;
    std::ifstream ifs("model/classes.txt");
    std::string line;
    while (getline(ifs, line)){
        labels.push_back(line);
    }

    // init video capture and image frame
    cv::Mat frame;
    cv::VideoCapture capture("sample.mp4");
    if (!capture.isOpened()){ 
        std::cerr << "Error opening video file\n";
        return -1;
    }

    // init model from .onnx file 
    // parameterize network to train on cpu since no gpu available 
    cv::dnn::Net net; net = cv::dnn::readNet("model/yolov5s.onnx");
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    // init program measurements
    auto start = std::chrono::high_resolution_clock::now();
    int frame_count = 0;
    int total_frames = 0;
    float fps = -1;

    while (capture.read(frame)){
        frame_count++;
        total_frames++;

        // format the input image to the model
        int row = frame.rows; 
        int col = frame.cols;
        int sides = MAX(col, row);
        cv::Mat input; input = cv::Mat::zeros(sides, sides, CV_8UC3);
        frame.copyTo(input(cv::Rect(0,0,col,row)));

        // Create blob from input image
        cv::Mat blob;
        cv::dnn::blobFromImage(input, blob, 1./255., cv::Size(INPUT_SIDES, INPUT_SIDES), cv::Scalar(), true, false);
        
        // Set inputs to network and extract output
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

        // iterate through non-maximal suppressed detections
        // and place corresponding rectangle and label
        for (int i = 0; i < output.size(); ++i){

            auto detection = output[i];
            auto box = detection.box;
            auto labelId = detection.labelId;
            const auto color = colors[labelId % colors.size()];
            cv::rectangle(frame, box, color, 3);
            cv::rectangle(frame, cv::Point(box.x, box.y - 20), cv::Point(box.x + box.width, box.y), color, cv::FILLED);
            cv::putText(frame, labels[labelId].c_str(), cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
        }

        // caculate fps every 30 frames
        if (frame_count >= 30){
            auto end = std::chrono::high_resolution_clock::now();
            fps = frame_count * 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            frame_count = 0;
            start = std::chrono::high_resolution_clock::now();
        }

        // display fps 
        if (fps > 0){
            std::ostringstream fps_label;
            fps_label << std::fixed << std::setprecision(2);
            fps_label << "FPS: " << fps;
            std::string fps_label_str = fps_label.str();

            cv::putText(frame, fps_label_str.c_str(), cv::Point(10, 25), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
        }

        cv::imshow("output", frame);

        if (cv::waitKey(1) != -1)
        {
            capture.release();
            std::cout << "finished by user\n";
            break;
        }
    }

    std::cout << "Total frames: " << total_frames << "\n";

    return 0;
}