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
//    Mat timg(image);
//    medianBlur(image, timg, 9);
//    Mat gray0(timg.size(), CV_8U), gray;

    vector<vector<Point> > contours;

    // find squares in every color plane of the image
//    for (int c = 0; c < 3; c++) {
//        int ch[] = {c, 0};
//        mixChannels(&timg, 1, &gray0, 1, ch, 1);
//
//        // try several threshold levels
        for (int l = 0; l < N; l++) {
//            // hack: use Canny instead of zero threshold level.
//            // Canny helps to catch squares with gradient shading
//            if (l == 0) {
//                // apply Canny. Take the upper threshold from slider
//                // and set the lower to 0 (which forces edges merging)
//                Canny(gray0, gray, 5, thresh, 5);
//                // dilate canny output to remove potential
//                // holes between edge segments
//                dilate(gray, gray, Mat(), Point(-1, -1));
//            } else {
//                // apply threshold if l!=0:
//                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
//                gray = gray0 >= (l + 1) * 255 / N;
//            }

            Mat imgBin = binarize(image);

            // find contours and store them all as a list
            findContours(imgBin, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
//            drawContours(imgBin, contours, -1, cv::Scalar(0, 255, 0),30, 8);
//
//            cv::namedWindow("contours", WINDOW_NORMAL);
//            imshow("contours",imgBin);
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
//    }
}

void pruneSquares(vector<vector<Point> > &squares, vector<RotatedRect> &rectangles, int dist) {
    vector<Rect> rectangles_tmp;
    vector<RotatedRect> rot_rectangles_tmp;

    for (int i = 0; i < squares.size(); i++) {
        //Remove rectangles
        vector<Point> sq = squares[i];
//        int isItSq = abs((sq[2].y - sq[0].y) - (sq[2].x - sq[0].x));
        double isItSq = norm(Mat(sq[0]), Mat(sq[1])) - norm(Mat(sq[1]), Mat(sq[2]));

        if (isItSq < 20) {
//            cout << "sq " << i << endl;
//            rectangles_tmp.emplace_back(Rect(sq[0], sq[2]));
//            rot_rectangles_tmp.emplace_back(RotatedRect(sq[0], sq[1], sq[2]));
                rot_rectangles_tmp.emplace_back(minAreaRect(sq));
        }
//        cout << isItSq << " " << squares[i] << endl;
    }

//    rectangles = rectangles_tmp;
    rectangles = rot_rectangles_tmp;

    cout << "sq found : " << rot_rectangles_tmp.size() << endl;
    rot_rectangles_tmp.clear();

    for (auto const &rect1: rectangles) {
        bool found = false;
        for (auto const &rect2: rectangles_tmp) {
//            if(rect2.contains(rect1.tl()) || rect2.contains(rect1.br())){
            Point center = rect1.center;
            if (center.inside(rect2)) {
                found = true;
                break;
            }
        }
        if (!found) {
            rot_rectangles_tmp.push_back(rect1);
        }
    }

    rectangles = rot_rectangles_tmp;
    cout << "sq found after : " << rectangles.size() << endl;

}

// the function draws all the squares in the image
void drawSquares(Mat &image, const vector<RotatedRect> &rectangles) {

    for (auto const &rect: rectangles) {
//        cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 3, LINE_AA);
        for (int i = 0; i < 4; i++) {
            Point2f vertices[4];
            rect.points(vertices);
            line(image, vertices[i], vertices[(i+1)%4], Scalar(0,255,0), 2);
        }
    }

    cv::namedWindow("image", WINDOW_NORMAL);
    imshow("image", image);
}

void cropRectangles(Mat& image, const vector<RotatedRect> &squares) {

    // find the average angle of the squares
    double angle = 90.0;
    for (auto const &rect: squares) {
        angle += rect.angle / squares.size();
    }
    cout << "Average angle : " << angle << endl;

    int counter = 0;
    // get rotation matrix for rotating the image around its center in pixel coordinates
    cv::Point2f center((image.cols-1)/2.0, (image.rows-1)/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle, center not relevant
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), image.size(), angle).boundingRect2f();
    // adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - image.cols/2.0;
    rot.at<double>(1,2) += bbox.height/2.0 - image.rows/2.0;

    cv::Mat dst;
    cv::warpAffine(image, dst, rot, bbox.size());
    cv::namedWindow("rotated_im.png", WINDOW_NORMAL);
    cv::imshow("rotated_im.png", dst);

//    drawSquares(dst, squares);

    vector<RotatedRect> rot_rectangles_tmp;
    transform(squares, rot_rectangles_tmp, rot);
    drawSquares(dst, rot_rectangles_tmp);



//        for (auto const &rect: squares) {
//            cv::Mat crop = image(rect);
////                imshow("crop" + to_string(counter), crop);
//            bool result = false;
//            try {
//                result = imwrite("../generated_images/" + to_string(counter) + ".png", crop);
//                counter++;
//            }
//            catch (const cv::Exception &ex) {
//                fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
//            }
//            if (result)
//                printf("Saved PNG file.\n");
//            else
//                printf("ERROR: Can't save PNG file.\n");
//        }
//    }


}

void getIcons(string filename, const vector<Rect>& rectangles){
    vector<int> lines;
    vector<int> columns;
    for(auto const &rect: rectangles){
        bool linefound = false;
        bool columnfound = false;
        Point center = (rect.br() + rect.tl()) * 0.5;
        for(auto const &line: lines){
            if(abs(line - center.y) < 25){
                linefound = true;
                std::replace (lines.begin(), lines.end(), line, (line + center.y)/2);
                break;
            }
        }
        if(!linefound){
            lines.emplace_back(center.y);
        }

        for(auto const &col: columns){
            if(abs(col - center.x) < 25){
                columnfound = true;
                std::replace (columns.begin(), columns.end(), col, (col + center.x)/2);
                break;
            }
        }
        if(!columnfound){
            columns.emplace_back(center.x);
        }
    }

    cout << "Centres lignes : " << endl;
    for(auto const &line: lines){
        cout << line << endl;
    }

    cout << "Centres columns : " << endl;
    for(auto const &col: columns){
        cout << col << endl;
    }

}


