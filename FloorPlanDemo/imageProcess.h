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


#define PI 3.14

struct Node{
	int index;
	Point vertex;
	vector<int> indexMap;
};

struct Edge{
	int index;
	int oppositeIndex;
	float span;
	bool sign;
	Point p1;
	Point p2;

};


extern std::vector<std::vector<cv::Point> > contours_poly;
extern int maxImg;

void imageShow(Mat frame, String name);

Mat colorExtract(Mat &src);

void erodeAndDilate(Mat &src, int erosion_size);

Mat imageProcess(Mat src, int picIndex);

void openCVProcess(int imgNum);