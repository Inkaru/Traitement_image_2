#include <image_treatment.hpp>
#include "pattern_matcher.hpp"

/**
 * @brief Return the name of a given icon by template matching
 * @param templ The image icon
 * @return The name of the icon
 */
string matchIcon(Mat& templ) {
    // Method used for matchTemplate
    int match_method = 4;// TM COEFF

    // Icons sheet used to find match
    Mat img = imread("../icons.png");
    Mat result;
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    result.create( result_rows, result_cols, CV_32FC1 );
    matchTemplate(binarize(img), binarize(templ), result, match_method);

    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;
    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
    if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED ) {
        matchLoc = minLoc;
    } else {
        matchLoc = maxLoc;
    }

    Point center(matchLoc.x + templ.cols/2, 0) ;
//
//    Mat img_display;
//    img.copyTo( img_display );
//    rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
//    rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
//
//
//    namedWindow( "image_window", WINDOW_NORMAL );
//    namedWindow( "result_window", WINDOW_NORMAL );
//    imshow( "image_window", img_display );
//    imshow( "result_window", result );
//    waitKey();

    if(center.x < 400 ) {
        return "accident";
    } else if(center.x < 600 ) {
        return "bomb";
    } else if(center.x < 800 ) {
        return "car";
    } else if(center.x < 1000 ) {
        return "casualty";
    } else if(center.x < 1200 ) {
        return "electricity";
    } else if(center.x < 1400 ) {
        return "fire";
    } else if(center.x < 1600 ) {
        return "fireBrigade";
    } else if(center.x < 1800 ) {
        return "flood";
    } else if(center.x < 2000 ) {
        return "gas";
    } else if(center.x < 2200 ) {
        return "injury";
    } else if(center.x < 2400 ) {
        return "paramedics";
    } else if(center.x < 2600 ) {
        return "person";
    } else if(center.x < 2800 ) {
        return "police";
    } else {
        return "roadBlock";
    }


}

/**
 * @brief Return the size of a given icon by template matching
 * @param ico The image icon analyzed
 * @return The string of the icon's size
 */
string matchSize(Mat& ico){
    int match_method = 4;   // TM COEFF

    // Icons sheet used to find match
    Mat img = imread("../size.png");
    Mat templ;

    // Crop the text of the icon
    Rect crop(Point(0, 0.80 * ico.rows),Point(ico.cols, ico.rows));
    templ = ico(crop);

    // Calculate the mean "color" of the image
    Scalar m = mean(binarize(templ),noArray());

    if(m[0] < 250) {
        Mat img_display;
        img.copyTo(img_display);

        Mat result;
        int result_cols = img.cols - templ.cols + 1;
        int result_rows = img.rows - templ.rows + 1;
        result.create(result_rows, result_cols, CV_32FC1);
        matchTemplate(img, templ, result, match_method);

        normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
        double minVal;
        double maxVal;
        Point minLoc;
        Point maxLoc;
        Point matchLoc;
        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
        if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED) { matchLoc = minLoc; }
        else { matchLoc = maxLoc; }

        rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8,
                  0);
        rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

        Point center(matchLoc.x + templ.cols / 2, 0);

//        namedWindow("image_window", WINDOW_NORMAL);
//        namedWindow("result_window", WINDOW_NORMAL);
//        imshow("image_window", img_display);
//        imshow("result_window", result);
//        waitKey();

        if(center.x < 450 ) {
            return "small";
        } else if(center.x < 750 ) {
            return "medium";
        } else {
            return "large";
        }
    } else {
        return("");
    }
}



