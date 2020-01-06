# Icon Ultra Matcher

This software is a project of the course "Traitement image et vidéo" at INSA Rennes. The goal of this  project is to extract and tag emergency services related icons from the NicIcon database.

## Getting started

### Prerequisites

To use this software you need to install the OpenCV and OpenCV-contrib libraries. 

### Installing

Compile the project using the `CMakeLists.txt` file. Works best with CLion IDE.

### Launching

There are two ways to run the program : 
- Test mode
Launch the executable with the command line parameter `test`. It will run the program on the pages located in base-test folder.
- Normal mode
Launch the executable without parameter. It will run the program on all folders named wxxx-scans.

## Method used

Square shapes are found in images thanks to pre-treatment (binarization) and countours detection. Pre-treatment is also useful during template matching. A special pre-treatment removes hand-drawing to enhance square recognition.

1. Images are set upright if necessary using its inner squares.
2. Squares corresponding to the icons are detected.
3. Icons (printed and drawned) are cropped thanks to the previous square recognition. 
4. Then images are tagged by [template matching](https://en.wikipedia.org/wiki/Template_matching). Only printed icons are recognized. Others are deduced by their positions. Scripter and page numbers are extracted from each filename.
5. Size is labelled with the same procedure than 4.

## Performance result

### Result on the test set

The image Pattern Matcher has a 100% success rate on the test set. However, the text matcher isn't evaluated as the test set doesn't contain the size information. Its success rate is higher than 95% on the training set.

### Quality of extraction

In order to hide the black border, the images are slightly cropped. This reduces the quality of extraction by hiding a part of the image if it has been drawn big enough to touch or overlap with the border.

### Cases of failure

As long as the input pages follow the same naming convention and format as the dataset, there aren't many cases of failure. The conditions that has to be met are : 

 * No red or black ink used for drawing that exceeds its border
 * Low scan inclination (< 1.5°)

In those conditions, the recognition can still fail in some extreme cases for close symbols like 'paramedics' and 'police'

## Authors

* **Pierre-antoine CABARET** - *Initial work*
* **Antonin GONTIER** - *Initial work*
* **Emilie HUMMEL** - *Initial work*
* **Arnaud ROGER** - *Initial work*

## License

This project is licensed under the same licenses as [OpenCV](https://github.com/opencv/opencv)