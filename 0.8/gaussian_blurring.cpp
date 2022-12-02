#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

// updated

int main(){

    // Read in Images
    cout << "* Enter Source Image File Name" << endl;
    cout << "-----------------------" << endl;
    string s; cin >> s;
    Mat img = imread(s);
    if ( img.empty() ){ 
          cout << "Error loading the image" << endl;
          return -1; 
     }

     // Select size of Gaussian Kernel, must be odd integer,
     // preferably between 1 and 1000 (to reduce runtime resulting from large kernel)
     cout << "* Enter size of Gaussian kernel to apply blurring:\n";
     cout << "The size of the kernel should be an odd number between 1 and 1000\n";
     cout << "-----------------------" << endl;
     int n; cin >> n;
     if (!n%2 || n < 1 || n > 1000){
        cout << "Error: the size of the kernel must be odd number between 1 and 1000";  
        return -1;
     }

     // Write output
    Mat blurredImg;
    GaussianBlur(img, blurredImg, Size(n,n), 0);
    imshow("Blurred Image", blurredImg);
    waitKey(0);
    imwrite(blurredImg, "gaussian_blur.png")


    destroyAllWindows();

    return 0;
}