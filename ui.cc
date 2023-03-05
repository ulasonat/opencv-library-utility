//
// Simple binary which gives a ui for the Image & Video libraries
//

#include <iostream>
#include <chrono>

#include "Image.h"
#include "Video.h"

using std::cout;
using std::cin;
using std::cerr;


    

// UI Functionalities for taking Input

static Image get_image_from_user(const std::string& name = "") {
    if (name.empty())
        cout << "* Enter Source Image File Name: ";
    else
        cout << "* Enter Source " << name << " Image File Name: ";

    std::string img_filename;
    cin >> img_filename;

    return Image(img_filename);
}

static Video get_video_from_user(const std::string& name = "") {
    if (!name.empty())
        cout << "* Enter Source Video File Name: ";
    else
        cout << "* Enter Source " << name << " Video File Name: ";

    std::string cap_filename;
    cin >> cap_filename;

    return Video(cap_filename);
}

template <typename Num_T>
static Num_T get_val_from_user(const std::string& name = "") {
    if (name.empty())
        cout << "* Enter a value: ";
    else
        cout << "* Enter the value for " << name << ": ";

    Num_T ret;
    cin >> ret;

    return ret;
}


// Image Processing Utility Use Cases


static Image alpha_blend(const Image& img_in) {
    Image other_img = get_image_from_user("image to blend");
    double other_weight = get_val_from_user<double>("weight for other img");

    other_img.fit_to_size(img_in);
    return img_in.alpha_blend(other_img, other_weight);
}

static Image edge_detect(const Image& img_in) {
    const int lower_threshold = get_val_from_user<int>("lower threshold");
    const int upper_threshold = get_val_from_user<int>("upper threshold");

    return img_in.edge_detect(lower_threshold, upper_threshold);
}

static Image gaussian_blur(const Image& img_in) {
    const int kernel_sz = get_val_from_user<int>("blur kernel size");

    return img_in.gaussian_blur(kernel_sz);
}

static Image homography_perspective(const Image& img_in) {
    cout << "* Enter 4 points and continue\n";
    const auto pts = img_in.collect_points();

    return img_in.create_homography(pts);
}

static Image get_object_features(const Image& img_in) {
    cout << "* Select object by entering points of an enclosing polygon\n";
    const auto pts = img_in.collect_points();

    const auto mask = img_in.get_mask(pts);

    const auto cofm = img_in.get_1st_moment(pts);
    cout << "Centre of mass: (" << cofm.x << ", " << cofm.y << ")\n";

    const auto area = img_in.get_0th_moment(pts);
    cout << "Area: " <<  area << "\n";
    
    return mask;
}

static Image proj_img(const Image& img_in) {
    cout << "* Enter 4 points to project to\n";
    const auto src_pts = img_in.collect_points();

    const Image src_img = get_image_from_user();
    const auto cutout_pts = src_img.collect_points();

    cout << "* Enter 4 points to cut and project onto\n";
     return img_in.proj_img(src_img, cutout_pts, src_pts);

}

static void threshold_options(){
    cout << "1: Binary Threshold\n";
    cout << "2: Inverted Binary Threshold\n";
    cout << "3: Truncated Threshold\n";
    cout << "4: Threshold to Zero\n";
    cout << "5: Inverted Threshold to Zero\n";
}

static Image threshold_mask(const Image& img_in) {

    threshold_options();
    const int type = get_val_from_user<int>("threshold type");
    const int val = get_val_from_user<int>("threshold value");

    return img_in.threshold(type, val);
}

static Image grayscale(const Image& img_in) {
    return img_in.grayscale();
}

static Image detection(const Image& img_in){
    return img_in.detection();
}

//------- Video Processing Utility Use Cases

static Video edge_detect(Video& cap_in) {
    const int lower_threshold = get_val_from_user<int>("lower threshold");
    const int upper_threshold = get_val_from_user<int>("upper threshold");

    return cap_in.edge_detect(lower_threshold, upper_threshold);
}

static Video gaussian_blur(Video& cap_in){
    const int kernel_sz = get_val_from_user<int>("blur kernel size");

    return cap_in.gaussian_blur(kernel_sz);
}

static Video homography_perspective(Video& cap_in){
    auto pts = cap_in.collect_points();
    return cap_in.create_homography(pts);
}

static Video threshold_mask(Video& cap_in) {

    threshold_options();
    const int type = get_val_from_user<int>("threshold type");
    const int val = get_val_from_user<int>("threshold value");

    return cap_in.threshold(type, val);
}

static Video grayscale(Video & cap_in){

    return cap_in.grayscale();
}

static Video tracking(Video& cap_in){

    return cap_in.track();
}

static Video detection(Video& cap_in){

    return cap_in.detection();
}

// Testing

void edge_detect_benchmark(){
    Image img1 = Image("sp500.png");
    auto totalTime = 0;
    for(int i=0; i<5000; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img1 = img1.edge_detect(100,200); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     cout << "totalTime in ms" << (totalTime/5000.0)/1000.0 << std::endl;
}

void gaussian_blur_benchmark(){
    Image img1 = Image("sp500.png");
    auto totalTime = 0;
    for(int i=0; i<500; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img1 = img1.gaussian_blur(15); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     cout << "Average time" << (totalTime/500.0)/1000.0 << "";
}

void gaussian_blur_benchmark(){
    Image img1 = Image("sp500.png");
    auto totalTime = 0;
    for(int i=0; i<500; i++){
        auto start = std::chrono::high_resolution_clock::now();
        img1 = img1.gaussian_blur(15); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
     cout << "Average time" << (totalTime/500.0)/1000.0 << "";
}

// needs points
void create_homography_benchmark(){
    Image img1 = Image("sp500.png");
    auto totalTime = 0;
    for(int i=0; i<500; i++){
        auto start = std::chrono::high_resolution_clock::now();
        // need pts
        img1 = img1.create_homography(); 
        auto end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        totalTime += time.count();
    }
    cout << "Average time" << (totalTime/500.0)/1000.0 << "";


}





// UI for Presentation

static void print_header() {
    cout << "----------------------------------------------\n";
    cout << "          OpenCV C++ library util\n";
    cout << "----------------------------------------------\n";
}

static void print_main_menu() {
    cout << "Select a processing option:\n";
    cout << "\ti: Image Processing\n";
    cout << "\tv: Video Processing\n";
    cout << "$ ";
}

static void print_image_menu() {
    cout << "Select a processing option:\n";
    cout << "\tR: reload image\n";
    cout << "\t1: display image\n";
    cout << "\t2: alpha blend\n";
    cout << "\t3: edge detect\n";
    cout << "\t4: gaussian blur\n";
    cout << "\t5: homography perspective\n";
    cout << "\t6: find object features\n";
    cout << "\t7: project image\n";
    cout << "\t8: threshold\n";
    cout << "\t9: grayscale\n";
    cout << "\tD: object detection\n";
    cout << "\t0: exit\n";
    cout << "$ ";
}

static void image_cases(){
    Image img = get_image_from_user();

    char opt;
    do {
        print_image_menu();
        cin >> opt;
        switch (opt) {
        case 'R':
        case 'r':
            img = get_image_from_user();
            break;
        case '1':
            img.show();
            break;
        case '2':
            img = alpha_blend(img);
            img.show();
            break;
        case '3':
            img = edge_detect(img);
            img.show();
            break;
        case '4':
            img = gaussian_blur(img);
            img.show();
            break;
        case '5':
            img = homography_perspective(img);
            img.show();
            break;
        case '6':
            img = get_object_features(img);
            img.show();
            break;
        case '7':
            img = proj_img(img);
            img.show();
            break;
        case '8':
            img = threshold_mask(img);
            img.show();
            break;
        case '9':
            img = grayscale(img);
            img.show();
            break;
        case 'D':
            img = detection(img);
            img.show();
            break;
        case '0':
            break;
        case 'T':
            gaussian_blur_benchmark();
            break;
        default:
            cerr << "Err: unhandled option\n";
            break;
        img.destroy_all_windows();
        }
    } while (opt != '0');

}

static void print_video_menu(){
    cout << "Select a processing option:\n";
    cout << "\tR: reload video\n";
    cout << "\t1: display video\n";
    cout << "\t2: edge detect\n";
    cout << "\t3: gaussian blur\n";
    cout << "\t4: homography perspective\n";
    cout << "\t5: threshold\n";
    cout << "\t6: grayscale\n";
    cout << "\tT: track\n";
    cout << "\tD: detect\n";
    cout << "\t0: exit\n";
    cout << "$ ";
}

static void video_cases(){
    Video cap = get_video_from_user();
    
    char opt;
    do {
        print_video_menu();
        cin >> opt;
        switch(opt){
        case '1':
            cap.show();
            break;
        case '2':
            cap = edge_detect(cap);
            break;
        case '3':
            cap = gaussian_blur(cap);
            break;
        case '4':
            cap = homography_perspective(cap);
            break;
        case '5':
            cap = threshold_mask(cap);
            break;
        case '6':
            cap = grayscale(cap);
            break;
        case 'T':
            break;
            cap = tracking(cap);
        case 'D':
            cap = detection(cap);
            break;
        }
    }  while (opt != '0');
}

int main() {

    print_header();
    print_main_menu();
    char opt;
    std::cin >> opt;
    switch(opt){
    case 'i':
        image_cases();
        break;
    
    case 'v':
        video_cases();
        break;
    }

    
}