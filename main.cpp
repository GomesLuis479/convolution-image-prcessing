// Core openCV and image read/write headers.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

// Data type to store elements of the deviation
// matrix for filtering.
struct deviationElem {
    int x, y;
};

// Used as a data type in getMeanAndVariance() function.
struct imageProperties{
    double mean, variance;
};

// Function declarations.
Mat readImage(int argc, char** argv);
Mat convertToGray(Mat& RGBimage);
Mat filerGrayImage(Mat& grayImage);
void displayImage(Mat& image, String windowName);

// Helper function to find parameters.
imageProperties getMeanAndVariance(Mat& image);

// Functions to calculate filtering quality parameters.
double SSI(Mat& original, Mat& filtered);
double MSE(Mat& original, Mat& filtered);
double PSNR(Mat& original, Mat& filtered);
double ESIH(Mat& original, Mat& filtered);
double ESIV(Mat& original, Mat& filtered);


int main(int argc, char** argv) {

    Mat inputImage = readImage(argc, argv);
    Mat grayImage = convertToGray(inputImage);
    Mat filteredImage = filerGrayImage(grayImage);

    displayImage(inputImage, "input image");
    displayImage(grayImage, "gray image");
    displayImage(filteredImage, "filtered image");

    double _SSI = SSI(grayImage, filteredImage);
    double _MSE = MSE(grayImage, filteredImage);
    double _PSNR = PSNR(grayImage, filteredImage);
    double _ESIH = ESIH(grayImage, filteredImage);
    double _ESIV = ESIV(grayImage, filteredImage);

    cout << endl << "------   RESULTS   ------" << endl << endl;

    cout << "SSI  : " << _SSI << endl;
    cout << "MSE  : " << _MSE << endl;
    cout << "PSNR : " << _PSNR << endl;
    cout << "ESIH : " << _ESIH << endl;
    cout << "ESIV : " << _ESIV << endl;

    cout << endl << "-------------------------" << endl;
    
    waitKey(0);                            
    return 0;
}


Mat readImage(int argc, char** argv) {

    // Check if path to image is passed through command line.
    if( argc != 2) {
        cout << "Image not passed as argument" << endl;
        exit(1);
    }

    Mat inputImage = imread(argv[1], CV_LOAD_IMAGE_COLOR);

    // Check for invalid file input.
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
 
    for(int i = 0; i < RGBimage.rows; i++) {
        for(int j = 0; j < RGBimage.cols; j++) {

            b_double = rgbPointer[RGBimage.step*i + 3*j ] ;
            g_double = rgbPointer[RGBimage.step*i + 3*j + 1];
            r_double = rgbPointer[RGBimage.step*i + 3*j + 2];

            gray = round( (r_double + g_double + b_double) / 3 );

            grayPointer[grayImage.step*i + j] = gray;

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

    // Read filter size and elements of the filter.
    cout << endl << "Enter the filter size: (must be an odd number and <= 11) ";
    cin >> n;

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            cout << endl << "Enter element ( " << i << ", " << j << " ) : ";
            cin >> filter[i][j]; 
        }
    }

    // Build deviation matrix.
    // This stores the (x, y) deviation of each filter 
    // element from the center element in terms of (x, y).

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            deviationMatrix[i][j].x = j - n/2;
            deviationMatrix[i][j].y = i - n/2;
        }
    }

    // e.g in a 0 indexed 3x3 filter
    // deviationMatrix[1][1] = (x, y) = (0, 0)
    // deviationMatrix[0][0] = (x, y) = (-1, -1)
    // deviationMatrix[2][2] = (x, y) = (1, 1)
    // deviationMatrix[0][1] = (x, y) = (0, -1)

    // The (x, y) values in the deviation matrix are used to
    // get the shift in index for filtering.
    

    // Initialize filtered image container and required pointers.
    Mat filteredImage(grayImage.rows, grayImage.cols, CV_8U, Scalar(0));
    unsigned char* grayPointer = (unsigned char*)grayImage.data;
    unsigned char* filteredPointer = (unsigned char*)filteredImage.data;

    int x_shift, y_shift, x_index, y_index;
    unsigned char temp;

    // Perform filtering.
    for(int i = 0; i < grayImage.rows; i++) {
        for(int j = 0; j < grayImage.cols; j++) {

            filterProduct = 0;

            for(int k = 0; k < n; k++) {
                for(int m = 0; m < n; m++) {
                    x_shift = deviationMatrix[k][m].x;
                    y_shift = deviationMatrix[k][m].y;

                    x_index = i + y_shift;
                    y_index = j + x_shift;

                    // Check if the shifted index is out of bounds for the given image.
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

    // Mean = E(x)
    // Variance = E(x^2) - Mean^2
    
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


double ESIH(Mat& original, Mat& filtered) {
    double sum1 = 0;
    double sum2 = 0;

    unsigned char* originalPointer = (unsigned char*)original.data;
    unsigned char* filteredPointer = (unsigned char*)filtered.data;

    double grayValue1, grayValue2, grayValue3, grayValue4;

    for(int i = 0; i < original.rows; i++) {
        for(int j = 0; j < original.cols - 1; j++) {

            // numerator
            grayValue1 = filteredPointer[filtered.step*i + (j+1) ];
            grayValue2 = filteredPointer[filtered.step*i + j ];

            sum1 += abs(grayValue1 - grayValue2);

            // denominator
            grayValue3 = originalPointer[original.step*i + (j+1) ];
            grayValue4 = originalPointer[original.step*i + j ];

            sum2 += abs(grayValue3 - grayValue4);

        }
    }

    double returnValue = sum1 / sum2;

    return returnValue;
}


double ESIV(Mat& original, Mat& filtered) {
    double sum1 = 0;
    double sum2 = 0;

    unsigned char* originalPointer = (unsigned char*)original.data;
    unsigned char* filteredPointer = (unsigned char*)filtered.data;

    double grayValue1, grayValue2, grayValue3, grayValue4;

    for(int j = 0; j < original.cols - 1; j++) {
        for(int i = 0; i < original.rows; i++) {

            // numerator
            grayValue1 = filteredPointer[filtered.step*i + j ];
            grayValue2 = filteredPointer[filtered.step*(i+1) + j ];

            sum1 += abs(grayValue1 - grayValue2);

            // denominator
            grayValue3 = originalPointer[original.step*i + j ];
            grayValue4 = originalPointer[original.step*(i+1) + j ];

            sum2 += abs(grayValue3 - grayValue4);

        }
    }

    double returnValue = sum1 / sum2;

    return returnValue;
}