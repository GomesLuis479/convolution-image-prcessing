#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cmath>


using namespace cv;
using namespace std;


struct deviationElem {
    int x, y;
};

struct imageProperties{
    double mean, variance;
};


Mat readImage(int argc, char** argv);
Mat convertToGray(Mat& RGBimage);
void displayImage(Mat& image, String windowName);
Mat filerGrayImage(Mat& grayImage);
imageProperties getMeanAndVariance(Mat& image);

double SSI(Mat& original, Mat& filtered);
double MSE(Mat& original, Mat& filtered);
double PSNR(Mat& original, Mat& filtered);

// test functions;
void printGrayValues(Mat& grayImage, String imageName);
Mat generateTestImage();


int main(int argc, char** argv) {

    // ------------------ test -------------

    Mat grayImage = generateTestImage();
    Mat filteredImage = filerGrayImage(grayImage);

    printGrayValues(grayImage, "input");
    printGrayValues(filteredImage, "filtered");

    // struct imageProperties a = getMeanAndVariance(grayImage);
    // cout << endl << a.mean << "  " << a.variance;

    double a = PSNR(grayImage, filteredImage);
    cout<<endl<<"PSNR: "<<a;

    // -------------------------------------


    // Mat inputImage = readImage(argc, argv);
    // Mat grayImage = convertToGray(inputImage);
    // Mat filteredImage = filerGrayImage(grayImage);
    // displayImage(inputImage, "input image");
    // displayImage(grayImage, "gray image");
    // displayImage(filteredImage, "filtered image");

    // double ssi = SSI(grayImage, filteredImage);
    // cout<<endl<<ssi;

    // waitKey(0);                            
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

            grayPointer[grayImage.step*i + j] = round(gray);

        }
    }

    return grayImage;
}

void displayImage(Mat& image, String windowName) {
    namedWindow(windowName, WINDOW_AUTOSIZE);
    imshow(windowName, image);
}

Mat filerGrayImage(Mat& grayImage) {
    int n; // filter size
    struct deviationElem deviationMatrix[11][11];
    double filter[11][11];
    double filterProduct = 0;

    // -------------  Read filter size and elements  -------------

    cout << endl << "Enter the filter size: (must be an odd number and <= 11) ";
    cin >> n;

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            cout << endl << "Enter element ( " << i << ", " << j << " ) : ";
            cin >> filter[i][j]; 
        }
    }

    // -----------------------------------------------------------




    // -------------  Build deviation matrix  -------------

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            deviationMatrix[i][j].x = j - n/2;
            deviationMatrix[i][j].y = i - n/2;
        }
    }
    
    // ---------------------------------------------------



    // initialize filtered image container and required pointers
    Mat filteredImage(grayImage.rows, grayImage.cols, CV_8U, Scalar(0));
    unsigned char* grayPointer = (unsigned char*)grayImage.data;
    unsigned char* filteredPointer = (unsigned char*)filteredImage.data;

    int x_shift, y_shift, x_index, y_index;
    unsigned char temp;
    for(int i = 0; i < grayImage.rows; i++) {
        for(int j = 0; j < grayImage.cols; j++) {

            filterProduct = 0;

            for(int k = 0; k < n; k++) {
                for(int m = 0; m < n; m++) {
                    x_shift = deviationMatrix[k][m].x;
                    y_shift = deviationMatrix[k][m].y;

                    x_index = i + y_shift;
                    y_index = j + x_shift;


                    if( (x_index<0) || (x_index>=grayImage.rows) || (y_index<0) || (y_index>=grayImage.cols) ){
                        continue;
                    }

                    temp = grayPointer[grayImage.step*x_index + y_index];
                    filterProduct += filter[k][m] * int(temp);

                }
            }

            if(filterProduct < 0) {
                filteredPointer[filteredImage.step*i + j] = 0;
            }
            else if(filterProduct > 255) {
                filteredPointer[filteredImage.step*i + j] = 255;
            }
            else {
                filteredPointer[filteredImage.step*i + j] = round(filterProduct);
            }
        }
    }

    return filteredImage;
}

imageProperties getMeanAndVariance(Mat& image) {
    struct imageProperties returnValue;

    unsigned char* imagePointer = (unsigned char*)image.data;

    double sum1 = 0; 
    double sum2 = 0;
    double grayValue;

    for(int i = 0; i < image.rows; i++) {
        for(int j = 0; j < image.cols; j++) {
            grayValue =  imagePointer[image.step*i + j ]  ;
            sum1 += grayValue;
            sum2 += grayValue * grayValue;
        }
    }

    double mean = sum1 / (image.rows * image.cols);
    double variance = ( sum2 / ( image.rows * image.cols ) ) - (mean*mean);

    returnValue.mean = mean;
    returnValue.variance = variance;

    return returnValue;

}

double SSI(Mat& original, Mat& filtered) {
    struct imageProperties originalProps = getMeanAndVariance(original);
    struct imageProperties filteredProps = getMeanAndVariance(filtered);

    double originalMean = originalProps.mean;
    double originalVariance = originalProps.variance;
    double filteredlMean = filteredProps.mean;
    double filteredVariance = filteredProps.variance;

    double returnValue = ( sqrt(filteredVariance) * originalMean ) / ( filteredlMean * sqrt(originalVariance) );

    return returnValue; 

}

double MSE(Mat& original, Mat& filtered) {
    double sum = 0;

    unsigned char* originalPointer = (unsigned char*)original.data;
    unsigned char* filteredPointer = (unsigned char*)filtered.data;

    double grayValue1, grayValue2;
    for(int i = 0; i < original.rows; i++) {
        for(int j = 0; j < original.cols; j++) {
            grayValue1 =  originalPointer[original.step*i + j ]  ;
            grayValue2 =  filteredPointer[filtered.step*i + j ]  ;

            sum += (grayValue1 - grayValue2) * (grayValue1 - grayValue2);
        }
    }

    double returnValue = sum / (original.rows * original.cols);

    return returnValue;
}

double PSNR(Mat& original, Mat& filtered) {
    double returnValue = 10 * log10( (255*255) / MSE(original, filtered));
    return returnValue;
}

void printGrayValues(Mat& grayImage, String imageName) {

    unsigned char* grayPointer = (unsigned char*)grayImage.data;

    unsigned char grayLevel;
    cout << endl << "------------   " << imageName << "   ------------" << endl;
    for(int i = 0; i < grayImage.rows; i++) {
        for(int j = 0; j < grayImage.cols; j++) {
            grayLevel = grayPointer[grayImage.step*i + j ] ;
            cout<<"  "<<int(grayLevel);
        }
        cout<<endl;
    }
}

Mat generateTestImage() {
    int rows = 3;
    int cols = 3;
    unsigned char test[3][3] = {
        {10, 10, 10},
        {100, 100, 100},
        {10, 10, 10}
    };

    Mat grayImage(rows, cols, CV_8U, Scalar(0));
    unsigned char* grayPointer = (unsigned char*)grayImage.data;

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            grayPointer[grayImage.step*i + j] = test[i][j];
        }
    }

    return grayImage;
}