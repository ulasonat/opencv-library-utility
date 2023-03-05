
#include <chrono>
#include "Image.h"
#include "Video.h"

// IMAGE BENCHMARKS


void load_benchmark(const int ITERATIONS){
    Image img;
    auto totalTime = 0;
    for(int i=0; i<ITERATIONS; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img = Image("sp500.png");
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     std::cout  << "Loading to Image object from file (average time, ms):  " << (totalTime/float(ITERATIONS))/1000.0 << std::endl;

}

//  benchmark blending two images
void alpha_blend_benchmark(const int ITERATIONS){
    Image img1 = Image("sp500.png");
    Image img2 = Image("times-square.png");
    auto totalTime = 0;
    for(int i=0; i<ITERATIONS; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img2.fit_to_size(img1);
        img1.alpha_blend(img2, 0.5); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     std::cout  << "Alpha Blend (average time, ms): " << (totalTime/float(ITERATIONS))/1000.0 << std::endl;
}

//  benchmark image detection for given set of thresholds
void edge_detect_benchmark(const int ITERATIONS){
    Image img1 = Image("sp500.png");
    auto totalTime = 0;
    const int T1 = 100;
    const int T2 = 200;
    for(int i=0; i<ITERATIONS; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img1.edge_detect(T1,T2); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     std::cout  << "Edge Detect (average time, ms):  " << (totalTime/float(ITERATIONS))/1000.0 << std::endl;
}

//  benchmark gaussian blurring for given blurring factor 15
//      number of iterations reduced to 500 due to convolutional computations at each pixel
void gaussian_blur_benchmark(const int ITERATIONS){
    Image img1 = Image("sp500.png");
    auto totalTime = 0;
    const int KERNEL_SZ = 15;
    for(int i=0; i<ITERATIONS; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img1.gaussian_blur(KERNEL_SZ); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     std::cout  << "Gaussian Blurring, (average time, ms):  " << (totalTime/float(ITERATIONS))/1000.0 << "\n";
}

//  benchmark homography perspective for pre-collected vector of points
void create_homography_benchmark(const int ITERATIONS){
    
    Image img1 = Image("sp500.png");
    std::vector<cv::Point> pnts = {       
            cv::Point(0,0), 
            cv::Point(700,0), 
            cv::Point(700, 1000),
            cv::Point(0, 1000)
    };
    auto totalTime = 0;
    for(int i=0; i<ITERATIONS; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img1.create_homography(pnts); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     std::cout  << "Create Homography (average time, ms):  " << (totalTime/float(ITERATIONS))/1000.0 << "\n";
}



// benchmark threshold for chose thresholding type and value 
void threshold_benchmark(const int ITERATIONS){
    Image img = Image("sp500.png");
    auto totalTime = 0;
    for(int i=0; i<ITERATIONS; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img.threshold(1, 100); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     std::cout  << "Average thresholded image elapsed time (ms)" << (totalTime/float(ITERATIONS))/1000.0 << "";
}


void grayscale_benchmark(const int ITERATIONS){
    Image img = Image("sp500.png");
    auto totalTime = 0;
    for(int i=0; i<ITERATIONS; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img.grayscale(); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     std::cout  << "Average grayscale image elapsed time (ms): " << (totalTime/float(ITERATIONS))/1000.0 << "";
}


void detection_benchmark(const int ITERATIONS){
    Image img = Image("sp500.png");
    auto totalTime = 0;
    for(int i=0; i<ITERATIONS; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img.detection(); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     std::cout  << "Average image detection elapsed time (ms): " << (totalTime/ITERATIONS)/1000.0 << "";
}


// VIDEO BENCHMARKS

void video_edge_detection_benchmark(){

    Video video = Video("sample.mp4");
    auto start = std::chrono::high_resolution_clock::now();
    video.edge_detect(100, 200);
    auto end = std::chrono::high_resolution_clock::now();
     auto time = std::chrono::duration_cast<std::chrono::seconds> (end - start);
     std::cout  << "Video edge detection execution time (s): " << time.count() << "\n";


}

void video_gaussian_blur_benchmark(){
    
    Video video = Video("sample.mp4");
    auto start = std::chrono::high_resolution_clock::now();
    video.gaussian_blur(15);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::seconds> (end - start);
     std::cout  << "Video blurring execution time (s): " << time.count() << "\n";

}


void video_threshold_benchmark(){
    
    Video video = Video("sample.mp4");
    auto start = std::chrono::high_resolution_clock::now();
    video.threshold(1, 100);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::seconds> (end - start);
    std::cout << "Video threshold execution time (s): " << time.count() << "\n";

}

int main() {

    // image runtime benchmarks over number of iterations  
    load_benchmark(500);
    edge_detect_benchmark(1000);
    gaussian_blur_benchmark(1000);
    alpha_blend_benchmark(1000);
    create_homography_benchmark(1000);
    
    // video benchmarks 
    video_edge_detection_benchmark();
    video_gaussian_blur_benchmark();
    video_threshold_benchmark();


    return 0;
}