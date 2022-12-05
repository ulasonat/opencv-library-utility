//
// Simple binary which gives a ui for the Image library
//

#include <iostream>

#include "Image.h"

using std::cout;
using std::cin;
using std::cerr;


static Image get_image_from_user(const std::string& name = "") {
    if (name.empty())
        cout << "* Enter Source Image File Name: ";
    else
        cout << "* Enter Source " << name << " Image File Name: ";

    std::string img_filename;
    cin >> img_filename;

    return Image(img_filename);
}

// use for numeric types
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

static Image display(const Image& img_in) {
    img_in.show();
    return img_in;
}

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

static Image homograph_perspective(const Image& img_in) {
    cout << "* Enter 4 points and continue\n";
    const auto pts = img_in.collect_points(4);

    return img_in.create_homography(pts);
}

static Image get_centre_of_mass(const Image& img_in) {
    cout << "* Enter 4 points for centre of mass\n";
    const auto pts = img_in.collect_points(4);

    const auto cofm = Image::get_polygon_mean(pts);
    cout << "Centre of mass: (" << cofm.x << ", " << cofm.y << ")\n";

    return img_in;
}

static Image proj_img(const Image& img_in) {
    cout << "* Enter 4 points to project to\n";
    const auto img_pts = img_in.collect_points(4);

    // const Image src_img = get_image_from_user("source image");
    const Image src_img("sp500.png");  // TODO: tmp nit

    cout << "* Enter 4 points to cut and project onto\n";
    const auto cutout_pts = src_img.collect_points(4);

    return img_in.proj_img(src_img, cutout_pts, img_pts);
}

static Image threshold_mask(const Image& img_in) {
    cout << "1: Binary Threshold\n";
    cout << "2: Inverted Binary Threshold\n";
    cout << "3: Truncated Threshold\n";
    cout << "4: Threshold to Zero\n";
    cout << "5: Inverted Threshold to Zero\n";
    const int type = get_val_from_user<int>("threshold type");
    const int val = get_val_from_user<int>("threshold value");

    return img_in.threshold(type, val);
}

static Image grayscale(const Image& img_in) {
    return img_in.grayscale();
}

static void print_header() {
    cout << "----------------------------------------------\n";
    cout << "          Opencv C++ library util\n";
    cout << "----------------------------------------------\n";
}

static void print_menu() {
    cout << "Select an option:\n";
    cout << "\tR: reload image\n";
    cout << "\t1: display image\n";
    cout << "\t2: alpha blend\n";
    cout << "\t3: edge detect\n";
    cout << "\t4: gaussian blur\n";
    cout << "\t5: homograph perspective\n";
    cout << "\t6: find centre of mass\n";
    cout << "\t7: project image\n";
    cout << "\t8: threshold\n";
    cout << "\t9: grayscale\n";
    cout << "\t0: exit\n";
    cout << "$ ";
}

int main() {
    print_header();

    Image img("times-square.png");  // TODO: tmp nit
    // Image img = get_image_from_user("main img");

    char opt;
    do {
        print_menu();
        cin >> opt;

        switch (opt) {
        case 'R':
        case 'r':
            img = get_image_from_user("new image");
            break;
        case '1':
            img = display(img);
            break;
        case '2':
            img = alpha_blend(img);
            break;
        case '3':
            img = edge_detect(img);
            break;
        case '4':
            img = gaussian_blur(img);
            break;
        case '5':
            img = homograph_perspective(img);
            break;
        case '6':
            img = get_centre_of_mass(img);
            break;
        case '7':
            img = proj_img(img);
            break;
        case '8':
            img = threshold_mask(img);
            break;
        case '9':
            img = grayscale(img);
            break;

        case '0':
            break;

        default:
            cerr << "Err: unhandled option\n";
            break;
        }

    } while (opt != '0');
}
