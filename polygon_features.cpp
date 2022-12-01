#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

vector<Point> srcPoints;

void SrcCallBackFunc(int event, int x, int y, int flags, void* userdata){
    cout << "Bound object by 4 point box" << endl;
     if  ( event == EVENT_LBUTTONDOWN && srcPoints.size() < 4){
          srcPoints.push_back(Point(x,y));
          cout << x << " " << y << endl;
     }
}

int main() {
    cout << "* Enter Source Image File Name" << endl;
    cout << "-----------------------" << endl;
    string s; cin >> s;
     Mat img = imread(s, IMREAD_COLOR);
     if ( img.empty() ) { 
          cout << "Error loading the image" << endl;
          return -1; 
     }
     namedWindow("My Window", 1);
     setMouseCallback("My Window", SrcCallBackFunc, NULL);
     imshow("My Window", img);
     waitKey(0);
     
     if ( srcPoints.size()  < 4 ){ 
          cout << "Error: select 4 Points to bound object\n";
          return -1; 
     } 

     int meanX=0;
     int meanY=0;
     for(int i=0; i<4; i++){
        meanX += (srcPoints[i].x)*0.25;
        meanY += (srcPoints[i].y)*0.25;
     }

    cout << "Object mean x value: " << x << endl;
    cout << "Object mean y value: " << y << endl; 
     destroyAllWindows();
    return 0;

}