//////////////////////////////////////////////////////////////////////////
// Option Images
// Projet
// Theme : reconnaisance d'icone avec opencv
// contenu : charge, traite, recadre, reconnait les cadres d'icone, découpe et reconnait.
// version : alpha 0.1
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "squares.hpp"
#include "sift_comparator.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

int main (void) {
    //Init vector of images, squares and rectangles
    vector<string> names;
    names.emplace_back("../00000.png");
    //names.emplace_back("../00000rotated.png");
    vector<vector<Point>> squares;
    vector<Rect> rectangles;
    vector<Rect> referenceIcons;

    for( int i = 0; i < names.size(); i++ )
    {
        squares.clear();
        //todo : replace lines above to sift on all drawings with the minimal reference icon set
        rectangles.clear();
        referenceIcons.clear();

        // CROPPING IMAGES AND ICONS
        cout << "Process image " << names[i] << endl;
        Mat image = imread(names[i], 1);
        if(image.empty())
        {
            cout << "Couldn't load " << names[i] << endl;
            continue;
        }
        //Set upright if necessary
        Mat uprImage;
        uprightImage(image, uprImage);
        //Detect all squares
        findSquares(uprImage, squares);
        //Remove useless squares
        pruneSquares(squares, rectangles);
        //Draw remaining squares on the image
        drawSquares(uprImage, rectangles);
        imwrite( "out.png", uprImage );
        //Generate images of remaining squares
        cropRectangles(uprImage, rectangles);
        //Extract reference icons
        getIcons(uprImage, rectangles, referenceIcons);

        // IMAGE RECOGNITION
        // Initialization
        vector<vector<KeyPoint>> keypoints;
        vector<Mat> descriptors;
        vector<String> names;
        initIcons(keypoints, descriptors, names);

        // Matching
        // debug / test
        Mat test = imread("../generated_images/0.png");
        cout << "Icon corresponding to 0.png is " << identifyIcon(test, descriptors, names) << endl;

        int c = waitKey();
        if((char)c == 27) {
            break;
        }
    }

 /*
    //TEST ROTATED IMAGE
    Mat image = imread("../00000rotated.png", 1);
    Mat dst;

    uprightImage(image, dst);

    vector<vector<Point>> initialSquares;
    vector<Rect> prunedSquares;
    vector<Rect> icons;

    findSquares(dst, initialSquares);
    pruneSquares(initialSquares, prunedSquares);
    drawSquares(dst,prunedSquares);
    cropRectangles(dst,prunedSquares);
    getIcons(dst, prunedSquares, icons);
*/

    int c = waitKey();
	return EXIT_SUCCESS;
}
