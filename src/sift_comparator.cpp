#include <image_treatment.hpp>
#include "sift_comparator.hpp"

/**
 * @brief Init the sift detector with reference icons
 * @param keypoints List of keypoints
 * @param Descriptor List of decriptor
 */
void initIcons(vector<vector<KeyPoint>>& keypoints, vector<Mat>& descriptors, vector<string>& labels){
    // Create SIFT detector
    Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create(400);

    vector<string> icons;
    glob("../icons", "", icons, false, true);

    // Parse icons
    for(auto const &name: icons){
        labels.push_back(string(name.begin() + 9, name.end() - 4));
        Mat mat = imread(name, IMREAD_GRAYSCALE);

        vector<KeyPoint> keypoint; // Keypoints of the current icon
        detector->detect(mat, keypoint); // Detect keypoints
        Mat descriptor; // Descriptor of the current icon
        detector->compute(mat, keypoint, descriptor); // Compute descriptor according to the keypoints and icon

        keypoints.push_back(keypoint); //todo : remove, possibly useless
        descriptors.push_back(descriptor);
    }
}

/**
 * @brief Find the icon matching the best a given image
 * @param img The image to recognize
 * @return Best score
 */
string identifyIcon(Mat& img){
    //Load references
    vector<string> icons;
    vector<vector<KeyPoint>> keypoints2;
    vector<Mat> descriptors2;
    initIcons(keypoints2, descriptors2, icons);

    prepIcon(img);

    // Detect keypoints and compute current tested image descriptor
    Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create(400);
    vector<KeyPoint> keypoint;
    Mat descriptor;
    detector->detectAndCompute(img, noArray(), keypoint, descriptor);

    int scores = 0;
    int maxIdx;
    string highest_score_name = "";
    for(size_t i = 0; i < descriptors2.size(); i++){ // Compare on all reference icons
        // Init matching tools
        Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
        vector<vector<DMatch>> knn_matches;
        matcher->knnMatch(descriptor, descriptors2[i], knn_matches, 2);

        const float ratio_thresh = 0.7f;
        std::vector<DMatch> good_matches;
        for (size_t i = 0; i < knn_matches.size(); i++)
        {
            if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
            {
                good_matches.push_back(knn_matches[i][0]);
            }
        }
        if(good_matches.size()>=scores){
            scores = good_matches.size();
            highest_score_name = icons[i];
            maxIdx = i;
        }
    }

    //DEBUG
    int MIN_MATCH_COUNT = 10;
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    vector<vector<DMatch>> knn_matches;
    matcher->knnMatch(descriptor, descriptors2[maxIdx], knn_matches, 2);

    const float ratio_thresh = 0.7f;
    std::vector<DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            good_matches.push_back(knn_matches[i][0]);
        }
    }

    Mat img_matches;

    if(good_matches.size() > MIN_MATCH_COUNT){

        vector<Point2f> pts_src;
        vector<Point2f> pts_dst;

        for(auto p: good_matches){
            pts_src.push_back(keypoint[p.queryIdx].pt);
            pts_dst.push_back(keypoints2[maxIdx][p.trainIdx].pt);
        }

        drawMatches( img, keypoint, imread("../icons/" + icons[maxIdx] + ".png", IMREAD_GRAYSCALE), keypoints2[maxIdx], good_matches, img_matches, Scalar::all(-1),
                     Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
        //-- Show detected matches
        imshow("Good Matches & Object detection", img_matches );

        Mat2f H = findHomography(pts_src, pts_dst, RANSAC);
        if(!H.empty()){
//            std::vector<Point2f> obj_corners(4);
//
//            obj_corners[0] = Point2f(0, 0);
//            obj_corners[1] = Point2f( (float)img.cols, 0 );
//            obj_corners[2] = Point2f( (float)img.cols, (float)img.rows );
//            obj_corners[3] = Point2f( 0, (float)img.rows );
//            std::vector<Point2f> scene_corners(4);
//            cout << "persp" << endl;
//            perspectiveTransform( obj_corners, scene_corners, H);
//            cout << "persp" << endl;
//
//            //-- Draw lines between the corners (the mapped object in the scene - image_2 )
//            line( img_matches, scene_corners[0] + Point2f((float)img.cols, 0),
//                  scene_corners[1] + Point2f((float)img.cols, 0), Scalar(0, 255, 0), 4 );
//            line( img_matches, scene_corners[1] + Point2f((float)img.cols, 0),
//                  scene_corners[2] + Point2f((float)img.cols, 0), Scalar( 0, 255, 0), 4 );
//            line( img_matches, scene_corners[2] + Point2f((float)img.cols, 0),
//                  scene_corners[3] + Point2f((float)img.cols, 0), Scalar( 0, 255, 0), 4 );
//            line( img_matches, scene_corners[3] + Point2f((float)img.cols, 0),
//                  scene_corners[0] + Point2f((float)img.cols, 0), Scalar( 0, 255, 0), 4 );
                cout << "good" << endl;
//            cv::warpPerspective(img, img_matches, H, img.size());
//            imshow("Test", img_matches );
//            waitKey();
        }
    }


    //-- Show detected matches
    imshow("Good Matches", img_matches );
    waitKey();
    cout << good_matches.size() << endl;


    cout << "Score " << scores << " with " << highest_score_name << endl;
    return highest_score_name;
}

void prepIcon(Mat& icon) {
    Point textOver[1][4];
    textOver[0][0] = Point(0, icon.size().height);
    textOver[0][1] = Point(icon.size().width, icon.size().height);
    textOver[0][2] = Point(icon.size().width, icon.size().height * 0.79);
    textOver[0][3] = Point(0, icon.size().height * 0.79);
    const Point* ppt[1] = {textOver[0]};
    int npt[1] = {4};
    fillPoly(icon, ppt, npt, 1, Scalar(255, 255, 255));
}

string matchIcon(Mat& templ) {

    // method used for matchTemplate
    int match_method = 4;   // TM COEFF

    // icons sheet used to find match
    Mat img = imread("../snippets3.png");
    img = binarize(img);

//    Mat img_display;
//    img.copyTo( img_display );

    Mat result;
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    result.create( result_rows, result_cols, CV_32FC1 );
    matchTemplate(img, binarize(templ), result, match_method);


    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;
    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
    if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED )
    { matchLoc = minLoc; }
    else
    { matchLoc = maxLoc; }

//    rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
//    rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

    Point center(matchLoc.x + templ.cols/2, 0) ;

//    cout << center.x << endl;
//    namedWindow( "image_window", WINDOW_NORMAL );
//    namedWindow( "result_window", WINDOW_NORMAL );
//    imshow( "image_window", img_display );
//    imshow( "result_window", result );
//
//    waitKey();

    if(center.x < 400 )
    { return "accident";}
    else if(center.x < 600 )
    { return "bomb";}
    else if(center.x < 800 )
    { return "car";}
    else if(center.x < 1000 )
    { return "casualty";}
    else if(center.x < 1200 )
    { return "electricity";}
    else if(center.x < 1400 )
    { return "fire";}
    else if(center.x < 1600 )
    { return "fireBrigade";}
    else if(center.x < 1800 )
    { return "flood";}
    else if(center.x < 2000 )
    { return "gas";}
    else if(center.x < 2200 )
    { return "injury";}
    else if(center.x < 2400 )
    { return "paramedics";}
    else if(center.x < 2600 )
    { return "person";}
    else if(center.x < 2800 )
    { return "police";}
    else
    { return "roadBlock";}



}

string matchSize(Mat& ico){
    int match_method = 4;   // TM COEFF

    // icons sheet used to find match
    Mat img = imread("../size.png");
    Mat templ;

    // crop the text of the icon
    Rect crop(Point(0, 0.80 * ico.rows),Point(ico.cols, ico.rows));
    templ = ico(crop);

    // calculate the mean "color" of the image
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

//        cout << center.x << endl;

//        namedWindow("image_window", WINDOW_NORMAL);
//        namedWindow("result_window", WINDOW_NORMAL);
//        imshow("image_window", img_display);
//        imshow("result_window", result);
//        waitKey();

        if(center.x < 450 )
        { return "small";}
        else if(center.x < 750 )
        { return "medium";}
        else
        { return "large";}

    } else {
        return("");
    }
}



