

img_runner:
	g++ $(pkg-config --cflags --libs /opt/homebrew/Cellar/opencv/4.6.0_1/lib/pkgconfig/opencv4.pc) -o3 -std=c++20 ui.cc -o img_runner