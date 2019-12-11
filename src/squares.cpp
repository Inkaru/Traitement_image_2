//
// Created by picabare on 04/12/19.
//

#include "squares.hpp"

// The "Square Detector" program.
// It loads several images sequentially and tries to find squares in
// each image

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle(Point pt1, Point pt2, Point pt0) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findSquares(const Mat &image, vector<vector<Point> > &squares, int thresh, int N) {
    squares.clear();

//s    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    //pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    //pyrUp(pyr, timg, image.size());


    // blur will enhance edge detection
    Mat timg(image);
    medianBlur(image, timg, 9);
    Mat gray0(timg.size(), CV_8U), gray;

    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for (int c = 0; c < 3; c++) {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for (int l = 0; l < N; l++) {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if (l == 0) {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 5, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1, -1));
            } else {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l + 1) * 255 / N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // test each contour
            for (size_t i = 0; i < contours.size(); i++) {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if (approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    isContourConvex(Mat(approx))) {
                    double maxCosine = 0;

                    for (int j = 2; j < 5; j++) {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if (maxCosine < 0.3)
                        squares.push_back(approx);
                }
            }
        }
    }
}

void pruneSquares(vector<vector<Point> > &squares, vector<Rect> &rectangles, int dist) {
    vector<Rect> rectangles_tmp;
    for (int i = 0; i < squares.size(); i++) {
        //Remove rectangles
        vector<Point> sq = squares[i];
        int isItSq = abs((sq[2].y - sq[0].y) - (sq[2].x - sq[0].x));
        if (isItSq < 20) {
//            cout << "sq " << i << endl;
            rectangles_tmp.emplace_back(Rect(sq[0], sq[2]));
        }
//        cout << isItSq << " " << squares[i] << endl;
    }

    rectangles = rectangles_tmp;
    cout << "sq found : " << rectangles.size() << endl;
    rectangles_tmp.clear();

    for (auto const &rect1: rectangles) {
        bool found = false;
        for (auto const &rect2: rectangles_tmp) {
//            if(rect2.contains(rect1.tl()) || rect2.contains(rect1.br())){
            Point center = (rect1.br() + rect1.tl()) * 0.5;
            if (center.inside(rect2)) {
                found = true;
                break;
            }
        }
        if (!found) {
            rectangles_tmp.push_back(rect1);
        }
    }

    rectangles = rectangles_tmp;
    cout << "sq found after : " << rectangles.size() << endl;

}

// the function draws all the squares in the image
void drawSquares(Mat &image, const vector<Rect> &rectangles) {

    for (auto const &rect: rectangles) {
        cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 3, LINE_AA);
    }

    imshow("show me what you got", image);
}

void generateImages(string filename, const vector<Rect> &rectangles) {
    int counter = 0;
    Mat image = imread(filename, 1);
    if (image.empty()) {
        cout << "Couldn't load " << filename << endl;
    } else {
        for (auto const &rect: rectangles) {
            cv::Mat crop = image(rect);
//                imshow("crop" + to_string(counter), crop);
            bool result = false;
            try {
                result = imwrite("../generated_images/" + to_string(counter) + ".png", crop);
                counter++;
            }
            catch (const cv::Exception &ex) {
                fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
            }
            if (result)
                printf("Saved PNG file.\n");
            else
                printf("ERROR: Can't save PNG file.\n");
        }
    }

}

void deleteSquares(string filename, const vector<Rect>& rectangles, int margin){
    
}