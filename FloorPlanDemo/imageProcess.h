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

struct Edge{
	int index;
	int oppositeIndex;
	float span;
	Point p1;
	Point p2;
	Edge(int _index, int _oppositeIndex, float _span, Point _p1, Point _p2) : index(_index), oppositeIndex(_oppositeIndex), span(_span), p1(_p1), p2(_p2) {}
};


extern std::vector<std::vector<cv::Point> > contours_poly;
extern int maxImg;

void imageShow(Mat frame, String name);

Mat colorExtract(Mat &src);

void erodeAndDilate(Mat &src, int erosion_size);

Mat imageProcess(Mat src, int picIndex);

void openCVProcess(int imgNum);