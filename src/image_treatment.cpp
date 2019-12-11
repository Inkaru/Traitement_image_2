#include "image_treatment.hpp"

Mat bina(const Mat& image) {

    Size taille(image.cols, image.rows);

    // Convert to greyscale
    Mat grayscale = Mat(taille, CV_8UC3);
    cvtColor(image, grayscale, COLOR_BGR2GRAY);

    // Applying gaussian filter
    Mat gaussfilter = Mat(taille, CV_8UC3);
    GaussianBlur(grayscale, gaussfilter, Size(3, 3), 0, 0, BORDER_DEFAULT);
    //imshow("Gaussien :",gaussfilter);


    // Applying a binary threshold to the image
    Mat binary = Mat(taille, CV_8UC3);
    int thresh = 220;
    int maxval = 256;
    threshold(gaussfilter, binary, thresh, maxval, THRESH_BINARY);
    //imshow("Threshold image", binary);

    // Applying morphomath opening --> not very useful, the pictures are too small
//        int operation = 2;      // 2: Opening; 3: Closing; 4: Gradient; 5: Top Hat; 6: Black Hat
//        int morph_elem = 1;     // 0: Rect - 1: Cross - 2: Ellipse
//        int morph_size = 2;     // Kernel size : 2n +1
//        Mat imMorphoMaths = Mat(taille, CV_8UC3);
//        Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1),
//                                            Point(morph_size, morph_size));
//        morphologyEx(imBin, imMorphoMaths, operation, element);
        //imshow("Morpho image", imMorphoMaths);

    return binary;
}