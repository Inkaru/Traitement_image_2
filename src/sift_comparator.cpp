#include "sift_comparator.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

/**
 * @brief Init the sift detector with reference icons
 */
void initIcons(vector<vector<KeyPoint>>& keypoints, vector<Mat>& descriptors, vector<String>& names){
    // Create SIFT detector
    Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create();

    // Parse icons
    for(int i = 0; i<7; i++){
        String path = "../generated_images/r" + to_string(i) + ".png";
        Mat mat = imread(path);

        vector<KeyPoint> keypoint; // Keypoints of the current icon
        detector->detect(mat, keypoint); // Detect keypoints
        Mat descriptor; // Descriptor of the current icon
        detector->compute(mat, keypoint, descriptor); // Compute descriptor according to the keypoints and icon

        keypoints.push_back(keypoint); //todo : remove, possibly useless
        descriptors.push_back(descriptor);
        names.push_back("r" + to_string(i));
    }
}

/**
 * @brief Find the icon matching the best a given image
 * @param img The image to recognize
 * @return Name of the closest reference icon
 */
String identifyIcon(const Mat& img, vector<Mat>& descriptors, vector<String>& names){
    // Detect keypoints and compute current tested image descriptor
    Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create();
    vector<KeyPoint> keypoint;
    detector->detect(img, keypoint);
    Mat descriptor;
    detector->compute(img, keypoint, descriptor);

    // Init matching tools
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    vector<vector<DMatch>> knn_matches; //

    int scores = 0;
    String highest_score_name = "";
    for(size_t i = 0; i < descriptors.size(); i++){ // Compare on all reference icons
        matcher->knnMatch(descriptor, descriptors[i], knn_matches, 2);
        
        const float ratio_thresh = 0.7f;
        std::vector<DMatch> good_matches;
        for (size_t i = 0; i < knn_matches.size(); i++)
        {
            if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
            {
                good_matches.push_back(knn_matches[i][0]);
            }
        }
        if(good_matches.size()>scores){
            scores = good_matches.size();
            highest_score_name = names[i];
        }
    }
    
    return highest_score_name;
}