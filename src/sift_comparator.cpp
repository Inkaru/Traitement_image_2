#include "sift_comparator.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"



void initIcons(vector<String> names){
    Ptr<cv::xfeatures2d::SIFT> detector = cv::xfeatures2d::SIFT::create();
    for(auto const &name: names){
        String path = "../sample/icons/"+name+".png";
        Mat mat = imread(path);

        std::vector<cv::KeyPoint> keypoint;
        detector->detect(mat, keypoint);
        Mat output;
        detector->compute(mat, keypoint, output);

        keypoints.push_back(keypoint);
        descriptors.push_back(output);
        names.push_back(name);
    }
}

String identifyIcon(const Mat& img){
        Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
        std::vector< std::vector<DMatch> > knn_matches;

        //Read image keypoints
        std::vector<cv::KeyPoint> keypoint;
        detector->detect(img, keypoint);
        Mat output;
        detector->compute(img, keypoint, output);

        int scores = 0;
        String highest_score_name = "";
        for(int i = 0; i<14; i++){
            matcher->knnMatch( output, descriptors[i], knn_matches, 2 );
            
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