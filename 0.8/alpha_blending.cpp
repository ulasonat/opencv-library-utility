#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

// updated

int main(){

    // Read in Images

    cout << "* Enter Source 1 Image File Name" << endl;
    cout << "-----------------------" << endl;
    string s1; cin >> s1;
    Mat src1 = imread(s1);

    cout << "* Enter Source 2 Image File Name" << endl;
    cout << "-----------------------" << endl;
    string s2; cin >> s2;
    Mat src2 = imread(s2);

    if ( src1.empty() || src2.empty() ) { 
      cout << "Error loading source image. Check that files are in the current directory." << endl;
      return -1; 
    } 

    // Resize src2 to src1 in order to combine images
    cv::resize(src2, src2, src1.size());
    
    // Combine Images
    Mat dst;
    double alpha; 
    cout << " Simple Linear Blender " << endl;
    cout << "-----------------------" << endl;
    cout << "* Enter alpha [0.0-1.0]: ";
    double input; cin >> input;
    if( input >= 0 && input <= 1 )
      { alpha = input; }
    double beta; beta =  1.0 - alpha ;
    addWeighted(src1, alpha, src2, beta, 0.0, dst);
    imshow( "Linear Blend", dst );
    waitKey(0);
    return 0;
  }