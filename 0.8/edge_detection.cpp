#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

// updated

int main(){

    // Read in Images
    cout << "* Enter Source 1 image file name" << endl;
    cout << "-----------------------" << endl;
    string s1; cin >> s1;
    Mat src1 = imread(s1);

    // Select threshold values
    cout << "* Enter value for lower edge threshold" << endl;
    cout << "-----------------------" << endl;
    int t1; cin >> t1;
    cout << "* Enter value for upper edge threshold" << endl;
    cout << "-----------------------" << endl;
    int t2; cin >> t2;

    // Write result to {file_name}-edge_detection.png
    Mat res;
    Canny(img, res, t1, t2);
    imshow("res",res);
    waitKey(0);
    imwrite("edge_detection.png");
    return 0;
}