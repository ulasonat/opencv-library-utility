
OPENCV_LIBS=$(shell pkg-config --cflags --libs /opt/homebrew/Cellar/opencv/4.6.0_1/lib/pkgconfig/opencv4.pc)

img_runner:
	g++ ${OPENCV_LIBS} -o3 -std=c++20 ui.cc -o proj_runner