#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat readImage(int argc, char** argv);
Mat convertToGray(Mat& RGBimage);
void displayImage(Mat& image, String windowName);

int main(int argc, char** argv) {

    Mat inputImage = readImage(argc, argv);
    Mat grayImage = convertToGray(inputImage);

    displayImage(inputImage, "input image");
    displayImage(grayImage, "gray image");

    waitKey(0);                                         

    return 0;
}

Mat readImage(int argc, char** argv) {

    // check if path to image is passed through command line
    if( argc != 2) {
        cout << "Image not passed as argument" << endl;
        exit(1);
    }

    Mat inputImage = imread(argv[1], CV_LOAD_IMAGE_COLOR);

    // Check for invalid input
    if(! inputImage.data ) {                             
        cout << "Could not open or find the image" << endl ;
        exit(1);
    }

    return inputImage;
}

Mat convertToGray(Mat& RGBimage) {
    
    Mat grayImage(RGBimage.rows, RGBimage.cols, CV_8U, Scalar(0));
    
    unsigned char* grayPointer = (unsigned char*)grayImage.data;
    unsigned char* rgbPointer = (unsigned char*)RGBimage.data;

    double r_double, g_double, b_double;
    unsigned char gray;
    unsigned char r, g, b;

    for(int i = 0; i < RGBimage.rows; i++) {
        for(int j = 0; j < RGBimage.cols; j++) {

            b = rgbPointer[RGBimage.step*i + 3*j ] ;
            g = rgbPointer[RGBimage.step*i + 3*j + 1];
            r = rgbPointer[RGBimage.step*i + 3*j + 2];

            r_double = r;
            g_double = g;
            b_double = b;

            gray = (r_double + g_double + b_double) / 3; 

            grayPointer[grayImage.step*i + j] = gray;

        }
    }

    return grayImage;
}

void displayImage(Mat& image, String windowName) {
    namedWindow(windowName, WINDOW_AUTOSIZE);
    imshow(windowName, image);
}