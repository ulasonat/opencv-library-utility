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
