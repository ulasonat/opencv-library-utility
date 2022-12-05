#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

using namespace cv;
using namespace std;

int main(){

    // init tracker
    Ptr<TrackerKCF> tracker = TrackerKCF::create();

    // read video
    cout << "Enter video to track object\n";
    string s; cin >> s;
    VideoCapture video(s);
    if (!video.isOpened()){
        cout << "Error: couldn't load video\n";
        return -1;
    }

    // init video frame and select bounding box
    Mat frame; video.read(frame);
    Rect box; box = selectROI(frame, false);
    rectangle(frame, box, Scalar(255,0,0),2,1);
    imshow("Tracker Frame 1", frame);
    tracker->init(frame, box);
    cout << "Press x to exit the program\n";

    // while video frame available
    // calculate & display updated bounding box and fps
    while(video.read(frame)){
        double timer = double(getTickCount());
        bool updated = tracker->update(frame, box);
        float fps = getTickFrequency() / ((double)getTickCount()-timer);
        if (updated){
            rectangle(frame, box, Scalar(255,0,0),2,1);
        }
        else{
            putText(frame, "Error: object tracking failed", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
        }
        putText(frame, "FPS: " + to_string(fps) , Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,255,0),2);
        imshow("Tracking", frame);
        int x; x = waitKey(1);
        if (x==120){
            return 1;
        }
    }
    return 0;
}   