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
string identifyIcon(const Mat& img){
    //Load references
    vector<string> icons;
    vector<vector<KeyPoint>> keypoints2;
    vector<Mat> descriptors2;
    initIcons(keypoints2, descriptors2, icons);

    // Detect keypoints and compute current tested image descriptor
    Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create(400);
    vector<KeyPoint> keypoint;
    Mat descriptor;
    detector->detectAndCompute(img, noArray(), keypoint, descriptor);

    int scores = 0;
    //int maxIdx;
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
            //maxIdx = i;
        }
    }

    /*
    //DEBUG
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
    drawMatches( img, keypoint, imread("../icons/" + icons[maxIdx] + ".png", IMREAD_GRAYSCALE), keypoints2[maxIdx], good_matches, img_matches, Scalar::all(-1),
                 Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    //-- Show detected matches
    imshow("Good Matches", img_matches );
    waitKey();
    cout << good_matches.size() << endl;
     */

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
