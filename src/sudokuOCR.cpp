/*
 * gaussianFilter.cpp
 *
 *  Created on: Aug 3, 2019
 *      Author: samuel
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <dirent.h>

using namespace cv;
using namespace std;

typedef vector<double> Array;
typedef vector<Array> Matrix;
typedef vector<Matrix> Image;

int main() {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("sudokuImg")) != NULL) {
		int t=0;

		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			string str1 = ent->d_name;
			if (str1 != "." && str1 != "..") {
				cout << str1 << endl;
				Mat image = imread("sudokuImg/"+str1, CV_LOAD_IMAGE_COLOR);   // Read the file

				if(! image.data ) {                             // Check for invalid input
					cout << "Could not open or find the image" << endl;
					return -1;
				}

				namedWindow( "Display window", WINDOW_NORMAL );// Create a window for display.
				resizeWindow("Display window", 600, 600);

				Mat greyMat;
				cvtColor(image, image, CV_BGR2GRAY);
				GaussianBlur(image, image, Size(5, 5), 0);

				adaptiveThreshold(image, image, 200, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 351, 20);


				Mat labels, stats, centroids;
				connectedComponentsWithStats(image, labels, stats, centroids);
				int max1=1, max2=0, ind1=0, ind2=0;
				for(int i=0; i<stats.rows; i++) {
					int h = stats.at<int>(Point(3, i));

					if (h > max1) {
						max2 = max1;
						ind2 = ind1;
						max1 = h;
						ind1 = i;
					}
					else if (h > max2){
						max2 = h;
						ind2 = i;
					}
			/*
					Scalar color(255,0,0);
					Rect rect(x,y,w,h);
					cv::rectangle(image, rect, color);
			*/
				}
				int x = stats.at<int>(Point(0, ind2));
				int y = stats.at<int>(Point(1, ind2));
				int wtot = stats.at<int>(Point(2, ind2))-20;
				int htot = stats.at<int>(Point(3, ind2));

				Rect myROI(x, y, wtot, htot);

				// Crop the full image to that image contained by the rectangle myROI
				// Note that this doesn't copy the data
				image = image(myROI);

				///*
				namedWindow("Display window", WINDOW_AUTOSIZE );

				connectedComponentsWithStats(image, labels, stats, centroids);

				int cellsize = wtot/9;
				string cells[9][9];
				for (int i=0; i<9; i++)
					for (int j=0; j<9; j++)
						cells[i][j]= "-1";

				string outText;
				Mat imagetemp;


				// Create Tesseract object
				tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();

				// Initialize tesseract to use English (eng) and the LSTM OCR engine.
				ocr->Init(NULL, "digits", tesseract::OEM_LSTM_ONLY);
				ocr->SetVariable("tessedit_char_whitelist","123456789");
				// Set Page segmentation mode to PSM_AUTO (3)
				ocr->SetPageSegMode(tesseract::PSM_AUTO);

				for(int i=0; i<stats.rows; i++) {

					int x = stats.at<int>(Point(0, i));
					int y = stats.at<int>(Point(1, i));
					int w = stats.at<int>(Point(2, i));
					int h = stats.at<int>(Point(3, i));

					if (w > 20 && w < 70 && h > 70 && h < 95) {

						Rect myROI(x-5, y-5, w+10, h+10);

						imagetemp = image(myROI);

						int tempy = y/cellsize;
						int tempx = x/cellsize;

						tesseract::TessBaseAPI tess;
						tess.Init(NULL, "digits", tesseract::OEM_LSTM_ONLY);
							// Set Page segmentation mode to PSM_AUTO (3)
						tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);

						tess.SetImage((uchar*)imagetemp.data, imagetemp.size().width, imagetemp.size().height, imagetemp.channels(), imagetemp.step1());
						tess.Recognize(0);
						const char* outText = tess.GetUTF8Text();
						cells[tempx][tempy] = outText;

					}
					Scalar color(255,0,0);
					Rect rect(x,y,w,h);
					cv::rectangle(image, rect, color);
				}

				ocr->End();

				ofstream myfile;
				myfile.open ("sudokuTxt/0.txt", ios::out);

				for (int i=0; i<9; i++)
					for (int j=0; j<9; j++)
						if (cells[i][j]!= "-1")
							myfile << i << " " << j << " " << cells[i][j];

				myfile.close();

				fstream file("sudokuTxt/0.txt", std::ios_base::in);

				int s[9][9] = {0};
				if (file.is_open()) {

					int l, m, n;
					while (file >> l >> m >> n)
						s[l][m] = n;

					for (int i=0; i<9; i++) {
						for (int j=0; j<9; j++) {
							cout << s[j][i] << " ";
						}
						cout << endl;
					}
					cout << endl;
					t++;
				}
				else cout << "Unable to open file" <<endl;

				file.close();

				imshow("Display window", image);
				waitKey(0);
			}
		}
	}
    return EXIT_SUCCESS;



}

