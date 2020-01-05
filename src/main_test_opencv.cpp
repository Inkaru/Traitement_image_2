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
#include "opencv2/core/utils/filesystem.hpp"

using namespace std;
using namespace cv;
using namespace cv::utils::fs;

int main (void) {
    //Perf
    auto timeStart = chrono::system_clock::now();

    //Init vector of images, squares and rectangles
    vector<string> names;
    vector<vector<Point>> squares;
    vector<Rect> rectangles;
    vector<Rect> referenceIcons;

    //Scan folder
    string filename;
    string iconeID;
    string scripter;
    int pageNumber;
    string page;

    if(!exists("../generated_images")){
        cout << "Create generated_images folder" << endl;
    }else{
        cout << "Empty generated_images folder" << endl;
        remove_all("../generated_images");
    }
    bool res = createDirectory("../generated_images");
    if(!res) {
        return EXIT_FAILURE;
    }

    vector<string> folderScripters;
    glob("../sample", "base-test", folderScripters, false, true);

    // Loop on the scripters
    for (auto const &fScript: folderScripters) {
        cout << "Enter folder " << fScript << endl;
        scripter = fScript.substr(11, 3);
        // loop on the 22 pages of each scripter
        for(pageNumber=0;pageNumber<22;pageNumber++) {
            if(pageNumber<10) {
                page = "0" + std::to_string(pageNumber);
            } else {
                page = std::to_string(pageNumber);
            }
            filename = "../sample/base-test/s" + page + "_00" + page + ".png";
            squares.clear();
            rectangles.clear();
            referenceIcons.clear();

            // CROPPING IMAGES AND ICONS
            cout << "Process image " << filename << endl;
            Mat image = imread(filename, 1);
            if(image.empty())
            {
                cout << "Couldn't load " << filename << endl;
                continue;
            }
            //Set upright if necessary
            Mat uprImage;
            uprightImage(image, uprImage);
            //Detect all squares
            findSquares(uprImage, squares, 1);
            //Remove useless squares
            pruneSquares(squares, rectangles);
            //Draw remaining squares on the image
            //drawSquares(uprImage, rectangles);
            //imwrite( "out.png", uprImage );
            //Generate images of remaining squares
            cropRectangles(uprImage, rectangles, scripter, page);
        }
    }

    auto timeEnd = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = timeEnd-timeStart;
    cout << endl << "Program finished in " << to_string(elapsed_seconds.count()) << "s" << endl;

    int c = waitKey();
	return EXIT_SUCCESS;
}
