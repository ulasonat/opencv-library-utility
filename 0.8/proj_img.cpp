

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

// work in progress

vector<Point> srcPoints;
vector<Point> dstPoints;

void SrcCallBackFunc(int event, int x, int y, int flags, void* userdata){
     if  ( event == EVENT_LBUTTONDOWN && srcPoints.size() < 4){
          srcPoints.push_back(Point(x,y));
          cout << x << " " << y << endl;
     }
}

void DstCallBackFunc(int event, int x, int y, int flags, void* userdata){
     if  ( event == EVENT_LBUTTONDOWN && dstPoints.size() < 4){
          dstPoints.push_back(Point(x,y));
          cout << x << " " << y << endl;
     }
}

int main(){
     // Read image from source file
     // Create a window & read in points using call 
     cout << "* Enter Source Image File Name" << endl;
    cout << "-----------------------" << endl;
    string in; cin >> in;
     Mat img = imread("sp500.png", IMREAD_COLOR);
     if ( img.empty() ){ 
          cout << "Error loading the image" << endl;
          return -1; 
     }
     namedWindow("My Window", 1);
     setMouseCallback("My Window", SrcCallBackFunc, NULL);
     imshow("My Window", img);
     waitKey(0);
     destroyWindow("My Window");

    // Read Corresponding Dest Image points
    cout << "* Enter Destination Image File Name" << endl;
    cout << "-----------------------" << endl;
    string d; cin >> d;
    Mat dst = imread("times-square.png", IMREAD_COLOR);
    if ( dst.empty() ) { 
          cout << "Error loading the image" << endl;
          return -1; 
     } 
    namedWindow("Destination", 1);
    setMouseCallback("Destination", DstCallBackFunc, NULL);
    imshow("Destination", dst);
    waitKey(0);
    destroyWindow("Destination");

     // Check homography computation requirements
     if ( srcPoints.size() < 4 && dstPoints.size() < 4 ){ 
          cout << "Error: 4 Points must be selected on both images to compute homography\n";
          return -1; 
     } 
     // Compute homography
     // Obtain src and dst masks to blend images 
     Mat h = findHomography(srcPoints, dstPoints);
     cv::Point sPts[4];
     for(int i=0;i<4;i++){
          sPts[i] = srcPoints[i];
     }
     Mat mask(img.rows, img.cols, CV_8UC3, Scalar(0,0,0));
     fillConvexPoly(mask, sPts, 4, Scalar(255, 255, 255));
     imshow("Mask", mask);
     waitKey(0);
     destroyWindow("Mask");
      destroyWindow("Mask");

     Mat srcMask;
     warpPerspective(mask, srcMask, h, size(dst));
     imshow("destination mask", srcMask);
     waitKey(0);
     destroyWindow("destination mask");

     Mat invMask;
     bitwise_not(srcMask, invMask);
     imshow("background mask", invMask);
     waitKey(0);
     destroyWindow("background mask");
     
     Mat dstMask;
     bitwise_and(invMask, dst, dstMask);
     imshow("destination background", dstMask);
     waitKey(0);
     destroyWindow("destination background");

     Mat warpedSrc; 
     Size sz = size(dst);
     warpPerspective(img, warpedSrc, h, sz);
     addWeighted( dst, 1, warpedSrc, 1, 0.0, dst);
     imshow("warped source", warpedSrc);
     waitKey(0);
     destroyWindow("warped source");
     
     // Show output
     Mat warpedMask;
     bitwise_and(warpedSrc, srcMask, warpedMask);
     imshow("warped mask", warpedMask);
     waitKey(0);
     destroyWindow("warped mask");

     Mat result;
     bitwise_or(dstMask, warpedSrc, result);
     imshow("result", result);
     waitKey(0);
     destroyWindow("warped result");

    
     destroyAllWindows();
    return 0;

}