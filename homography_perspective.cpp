#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

vector<Point> srcPoints;

void SrcCallBackFunc(int event, int x, int y, int flags, void* userdata){
     if  ( event == EVENT_LBUTTONDOWN && srcPoints.size() < 4){
          srcPoints.push_back(Point(x,y));
          cout << x << " " << y << endl;
     }
}

int main(){
     // Read image from source file
     // select 4 points.
     // Warp those 4 points to full size image
     cout << "* Enter Source Image File Name" << endl;
    cout << "-----------------------" << endl;
    string s; cin >> s;
     Mat img = imread(s, IMREAD_COLOR);
     if ( img.empty() ){ 
          cout << "Error loading the image" << endl;
          return -1; 
     }
     namedWindow("My Window", 1);
     setMouseCallback("My Window", SrcCallBackFunc, NULL);
     imshow("My Window", img);
     waitKey(0);
     

     // Check homography computation requirements
     if ( srcPoints.size()  < 4 ){ 
          cout << "Error: 4 Points must be selected on both images to compute homography\n";
          return -1; 
     } 

     vector<Point> dstPoints;
    dstPoints.push_back(Point(0, 0));
    dstPoints.push_back(Point(img.cols, 0));
    dstPoints.push_back(Point(img.cols, img.rows));
    dstPoints.push_back(Point(0, img.rows));
     // Compute homography
     // Obtain src and dst masks to blend images 
     Mat h = findHomography(srcPoints, dstPoints);
    Mat dst; 
    warpPerspective(img, dst, h, size(img));
    imshow("img", dst);
    waitKey(0);
    
     destroyAllWindows();
    return 0;

}