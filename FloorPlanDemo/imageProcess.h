#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <ctime>

using namespace cv;

struct Node{
	int index;
	Point vertex;
	vector<int> indexMap;
};


extern std::vector<std::vector<cv::Point> > contours_poly;
extern int maxImg;

void imageShow(Mat frame, String name);

Mat colorExtract(Mat &src);

void erodeAndDilate(Mat &src, int erosion_size);

Mat imageProcess(Mat src, int picIndex);

void openCVProcess(int imgNum);