#ifndef PROJET_OPENCV_CMAKE_IMAGE_TREATMENT_HPP
#define PROJET_OPENCV_CMAKE_IMAGE_TREATMENT_HPP
#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

Mat binarize(const Mat& image);
Mat removeDrawings(const Mat& image);

#endif