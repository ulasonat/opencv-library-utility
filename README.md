## Overview

This program offers various functionalities to process images & videos using OpenCV. These functionalities include transformation of image & video to grayscale, edge detection, Gaussian blurring, thresholding, motion tracking, and object detection. 

## Requirements 
1. C++20 Compiler
2. CMake: https://cmake.org/install/
3. OpenCV: Version 4.6.0_1. Download and Install the latest version of OpenCV for your respective machine from https://opencv.org/releases/

## Setup

### For macOS:
Firstly, install homebrew from https://brew.sh/. 

1. Install OpenCV: Run `brew install opencv`. Check that brew has been installed at /opt/homebrew/Cellar/opencv/4.6.0_1/lib/pkgconfig/opencv4.pc

Alternatively, do the following:
1. Extract OpenCV from the downloaded file.
2. Move OpenCV to the proper library directory: `/opt/homebrew/Cellar/opencv/4.6.0_1/lib/pkgconfig/opencv4.pc`

### For Both Windows and macOS:
1. Extract our project from the Github repository or a zip file. https://github.com/vicentefarias/4995_project/tree/Final
2. Change into the project directory.
3. Run `make` from the project directory and ensure the code compiles (this may require checking library path directory in Makefile).

## Usage
Create a `Image` or `Video` object using a file path or an instance of `cv::VideoCapture`. After initializing the `Video` object, you can call any of the following methods to apply a specific transformation to the them:
- `grayscale`: converts the image/video to grayscale
- `edge_detect`: applies edge detection to the image/video
- `gaussian_blur`: applies Gaussian blurring to the image/video
- `threshold`: applies thresholding to the image/video
- `track`: tracks an object in the image/video using OpenCV's KCF tracker
- `detection`: detects objects in the image/video using YOLOv5 object detection model

## Documentation
Please find the *documentation.pdf* included.

## Special Thanks to Dr. Bjarne Stroustrup
As students working on this project, we were incredibly fortunate to have the opportunity to be mentored by such an esteemed figure.

Throughout the project, Dr. Stroustrup provided invaluable guidance on a range of topics, including software design, code optimization, and debugging techniques. His extensive knowledge and expertise allowed us to learn practical skills that are not typically covered in a traditional classroom setting. We often found ourselves surprised at the depth and breadth of his insights, and we were grateful for the opportunity to learn from one of the industry's most seasoned experts.
