#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat image; // image container
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }


    // first pointers are uint_8
    unsigned char *input = (unsigned char*)(image.data);

    
    // unsigned char *out = (unsigned char*)(test.data);

    //cout<<"size of test: rows = "<<test.rows<<" cols = "<<test.cols<<" step =  " << test.step << " step image = "<<image.step;

    
    unsigned char r,g,b;
    for(int i = 0;i < image.rows ;i++){
			for(int j = 0;j < image.cols ;j++){
                 
                b = input[image.step * i + 3*j ] ;
                g = input[image.step * i + 3*j + 1];
                r = input[image.step * i + 3*j + 2];

                // out[test.step * i + j ] = (uint8_t)125;
                input[image.step * i + 3*j ] = 255;
                input[image.step * i + 3*j + 1] = 0;
                input[image.step * i + 3*j + 2] = 0;
                // b = out[test.step * i + j ] ;
                // g = 0; //out[test.step * i + j + 1];
                // r = 0; //out[test.step * i + j + 2];



                //if(r != 255 || g != 255 || b != 255 )
                //cout<<"at i,j= "<<i<<", "<<j<<" "<<"( "<<int(r)<<", "<<int(g)<<", "<<int(b)<<" )\n";
                //cout<<double(r+g+b)/3<<"\n";

                
            }
        }


    
    // namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    // imshow( "Display window", image );
    // waitKey(0);                                          // Wait for a keystroke in the window



    Mat test(image.rows, image.cols, CV_8U, Scalar(125));
    unsigned char* testPtr = (unsigned char*)test.data;


    int test_step = test.step;
    for(int i = 0; i < test.rows; i++) {
        for(int j = 0; j < test.cols; j++) {
            unsigned char val = testPtr[test_step*i + j];
            cout<<"at i,j= "<<i<<", "<<j<<" = "<<int(val)<<"\n";

            // changing the value
            testPtr[test_step*i + j] = 200;
        }
    }

    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", test );
    waitKey(0);                                          // Wait for a keystroke in the window

    return 0;
}