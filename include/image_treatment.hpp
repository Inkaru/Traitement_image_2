#ifndef PROJET_OPENCV_CMAKE_IMAGE_TREATMENT_HPP
#define PROJET_OPENCV_CMAKE_IMAGE_TREATMENT_HPP
#include <iostream>
using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

Mat bina(const Mat& image);

#endif