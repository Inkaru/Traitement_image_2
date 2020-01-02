#include "image_treatment.hpp"

/**
 * @brief Binarize (black/white) an image with greyscale, gaussian and threshold operations
 * @param image The given image
 * @return The binarized image
 */
Mat binarize(const Mat& image) {
    Size size(image.cols, image.rows);

    // Convert to greyscale
    Mat grayscale = Mat(size, CV_8UC3);
    cvtColor(image, grayscale, COLOR_BGR2GRAY);

    // Applying gaussian filter
    Mat gaussfilter = Mat(size, CV_8UC3);
    GaussianBlur(grayscale, gaussfilter, Size(3, 3), 0, 0, BORDER_DEFAULT);
    //imshow("Gaussian filtered image :",gaussfilter);

    // Applying a binary threshold to the image
    Mat binary = Mat(size, CV_8UC3);
    int thresh = 220;
    int maxval = 256;
    threshold(gaussfilter, binary, thresh, maxval, THRESH_BINARY);
    //imshow("Threshold image", binary);

    /*
    // Applying morphomath opening --> not very useful, the pictures are too small
    int operation = 2;      // 2: Opening; 3: Closing; 4: Gradient; 5: Top Hat; 6: Black Hat
    int morph_elem = 1;     // 0: Rect - 1: Cross - 2: Ellipse
    int morph_size = 2;     // Kernel size : 2n +1
    Mat imMorphoMaths = Mat(size, CV_8UC3);
    Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
    morphologyEx(imBin, imMorphoMaths, operation, element);
    //imshow("Morpho image", imMorphoMaths);
    */

    return binary;
}

/**
 * @brief Binarize (black/white) an image and remove all drawings (it keeps the squares)
 * @param image The given image
 * @return The binarized image
 */
Mat removeDrawings(const Mat& image) {
    Size size(image.cols, image.rows);

    // Convert to greyscale
    Mat grayscale = Mat(size, CV_8UC3);
    cvtColor(image, grayscale, COLOR_BGR2GRAY);
//    imshow("Grayscale",grayscale);

    // Applying a binary threshold to the image
    Mat binary = Mat(size, CV_8UC3);
    int thresh = 150;
    int maxval = 256;
    threshold(grayscale, binary, thresh, maxval, THRESH_BINARY);

    namedWindow("Threshold image", WINDOW_NORMAL);
    imshow("Threshold image", binary);

    return binary;
}