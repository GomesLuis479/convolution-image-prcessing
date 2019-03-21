#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    if( argc != 2) {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat inputImage = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    unsigned char* inputPointer = (unsigned char*)inputImage.data;

    if(! inputImage.data ) {                             // Check for invalid input
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }


    // output container
    Mat outputImage(inputImage.rows, inputImage.cols, CV_8U, Scalar(0));
    unsigned char* outputPointer = (unsigned char*)outputImage.data;

    double r_double, g_double, b_double;
    unsigned char gray;
    for(int i = 0; i < inputImage.rows; i++) {
        for(int j = 0; j < inputImage.cols; j++) {


            unsigned char b = inputPointer[inputImage.step*i + 3*j ] ;
            unsigned char g = inputPointer[inputImage.step*i + 3*j + 1];
            unsigned char r = inputPointer[inputImage.step*i + 3*j + 2];

            r_double = r;
            g_double = g;
            b_double = b;

            gray = (r_double + g_double + b_double) / 3; 

            outputPointer[outputImage.step*i + j] = gray;


        }
    }


    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", outputImage );
    waitKey(0);                                          // Wait for a keystroke in the window
    
    return 0;
}