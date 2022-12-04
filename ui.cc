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

static void print_header() {
    cout << "----------------------------------------------\n";
    cout << "          Opencv C++ library util\n";
    cout << "----------------------------------------------\n";
}

static void print_menu() {
    cout << "Select an option:\n";
    cout << "\t1: display image\n";
    cout << "\t2: alpha blend\n";
    // TODO add the others
    cout << "\t0: exit\n";
    cout << "$ ";
}

int main() {
    print_header();

    Image img = get_image_from_user("main img");

    int opt;
    do {
        print_menu();
        cin >> opt;

        switch (opt) {
        case 1:
            img = display(img);
            break;
        case 2:
            img = alpha_blend(img);
            break;

        default:
            cerr << "Err: unhandled option\n";
            break;
        }

    } while (opt != 0);
}
