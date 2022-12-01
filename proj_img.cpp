#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

// still need to finish
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
    string s; cin >> s;
     Mat img = imread(s, IMREAD_COLOR);
     if ( img.empty() ) 
     { 
          cout << "Error loading the image" << endl;
          return -1; 
     }
     namedWindow("Source", 1);
     setMouseCallback("Source", SrcCallBackFunc, NULL);
     imshow("Source Image", img);
     waitKey(0);

    // Read Corresponding Dest Image points
    cout << "* Enter Destination Image File Name" << endl;
    cout << "-----------------------" << endl;
    string d; cin >> d;
    Mat dst = imread(d, IMREAD_COLOR);
    if ( dst.empty() ) { 
          cout << "Error loading the image" << endl;
          return -1; 
     } 
    namedWindow("Destination", 1);
    setMouseCallback("Destination", DstCallBackFunc, NULL);
    imshow("Destination Image", dst);
    waitKey(0);

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
     Mat srcMask;
     warpPerspective(mask, srcMask, h, size(dst));
     bitwise_not(srcMask, srcMask);
     Mat dstMask;
     bitwise_and(srcMask, dst, dstMask);
     imshow("dstMask", dstMask);
     waitKey(0);
     Mat invMask;
     bitwise_not(srcMask, invMask);
    
    // Warp perspective
     Mat warpedDst; 
     Size s = size(dst);
     warpPerspective(img, warpedDst, h, s);
     addWeighted( dst, 1, warpedDst, 1, 0.0, dst);


     // Show output
     Mat warpedSrc;
     bitwise_and(warpedDst, invMask, warpedSrc);
     imshow("warpedSrc", warpedSrc);
     waitKey(0);

     Mat result;
     bitwise_or(dstMask, warpedSrc, result);
     imshow("result", result);
     waitKey(0);

    
     destroyAllWindows();
    return 0;

}