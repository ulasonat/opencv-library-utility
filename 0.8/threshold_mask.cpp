#include <opencv2/opencv.hpp>
#include <stdlib.h>
using namespace std;
using namespace cv;

int main(){
   // Read in Images
    cout << "* Enter Source 1 Image File Name" << endl;
    cout << "-----------------------" << endl;
    string s; cin >> s;
    Mat img = imread(s);
    if (img.empty()){
        cout << "Unable to read image\n";
        return -1;
    }
    imshow("Color Image", img);
    waitKey(0);
    destroyWindow("Color Image");
    
    // Convert image M to grayscale
    Mat grayM; cvtColor(img, grayM, COLOR_BGR2GRAY);
    imshow("Grayscale Image", grayM);
    waitKey(0);
    destroyWindow("Grayscale Image");
   
    // Ask for a threshold type and value arguments
    // Set default if not in range
    cout << "Types of image pixel processing thresholds- \n";
    cout << "1: Binary Threshold\n";
    cout << "2: Inverted Binary Threshold\n";
    cout << "3: Truncated Threshold\n";
    cout << "4: Threshold to Zero\n";
    cout << "5: Inverted Threshold to Zero\n";
    cout << "* Select type of threshold [1-5]:\n";
    cout << "-----------------------" << endl;
    int threshType; cin >> threshType;
    if (threshType < 1 || threshType > 5 ){
        cout << "Threshold type must be between 1 and 5.\n";
        return -1;
    }

    cout << "* Select pixel threshold value [0-255]:\n";
    cout << "-----------------------" << endl;
    int threshVal; cin >> threshVal;
    if (threshVal < 0 || threshType > 255 ){
        cout << "Pixel threshold must be between 0 and 255.\n";
        return -1;
    }

    Mat dst;   
    threshold(grayM,  dst, threshVal, 255, threshType);
    imshow("Thresholded Image", dst);
    waitKey(0);
    destroyWindow("Thresholded Image");
  
    destroyAllWindows();
    return 0;
}