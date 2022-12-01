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
     cout << "* Enter size of Gaussian kernel to apply blurring:\n";
     cout << "The size of the kernel should be an odd number between 1 and 1000\n";
     cout << "-----------------------" << endl;
     int n; cin >> n;
     if (!n%2 || n < 1 || n > 1000){
        cout << "Error: the size of the kernel must be odd number between 1 and 1000"
        return -1;
     }
    Mat blurredImg;
    GaussianBlur(img, blurredImg, Size(n,n), 0);

    imshow("Blurred Image", blurredImg);
    waitKey(0);


    destroyAllWindows();

    return 0;
}