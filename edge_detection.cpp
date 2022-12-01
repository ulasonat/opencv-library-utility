#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

int main(){

    cout << "* Enter Source Image File Name" << endl;
    cout << "-----------------------" << endl;
    string s; cin >> s;
    Mat img = imread(s);
    if ( img.empty() ){ 
            cout << "Error loading the image" << endl;
            return -1; 
        }
    cout << "* Enter Value for lower edge threshold" << endl;
    cout << "-----------------------" << endl;
        int t1; cin >> t1;
        cout << "* Enter Value for upper edge threshold" << endl;
    cout << "-----------------------" << endl;
        int t2; cin >> t2;

    Mat res;
    Canny(img, res, t1, t2);
    imshow("res",res);
    waitKey(0);
    return 0;
}