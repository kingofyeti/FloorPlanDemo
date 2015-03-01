//  ---------------------------------------------------------------------------
//
//  @file       FloorPlanDemo
//  @tools used: 
//              AntTweakBar: http://anttweakbar.sourceforge.net/doc
//              OpenGL:      http://www.opengl.org
//				OpenCV:		 http://www.opencv.org
//              GLUT:        http://opengl.org/resources/libraries/glut
//  
//  @author     Yeqi Wang
//  @date       02/25/2015
//
//  ---------------------------------------------------------------------------

#include "imageProcess.h"

std::vector<std::vector<cv::Point> > contours_poly;
int maxImg;

void imageShow(Mat frame, String name){
	Mat img = frame;
	if (img.empty())
	{
		std::cout << "error";
		return;
	}
	imshow(name, img);
}

Mat colorExtract(Mat &src){
	Mat dst, res;
	cvtColor(src, dst, COLOR_BGR2HSV);
	Scalar hsv_min(30, 50, 50);
	Scalar hsv_max(90, 255, 255);
	inRange(dst, hsv_min, hsv_max, res);
	return res;
}

void erodeAndDilate(Mat &src, int erosion_size){
	int erosion_type = MORPH_ELLIPSE;
	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));
	erode(src, src, element);
	dilate(src, src, element);
}

void lineRefinement(Point &p1, Point &p2, float refinRatio){
	if (abs(p1.x - p2.x) < refinRatio){
		p2.x = p1.x;
		std::cout << "get" << std::endl;
	}
	if (abs(p1.y - p2.y) < refinRatio){
		p2.y = p1.y;
		std::cout << "get" << std::endl;
	}
}

vector<Node> contoursToMap(vector<vector<Point>> &contours, float erosionRatio){
	vector<Node> nodeMap;
	int preIndex;
	for (int i = 0; i < contours.size(); i++){
		Node first;
		vector<int> fIndexmap;
		first.indexMap = fIndexmap;
		first.vertex = contours[i][0];
		first.index = nodeMap.size();
		nodeMap.push_back(first);
		preIndex = first.index;
		std::cout << i << ": " << contours[i].size() << std::endl;
		for (int j = 1; j < contours[i].size(); j++){
			int sign = false;
			int k;
			Point p1 = contours[i][j];
			for (k = 0; k < nodeMap.size(); k++){
				Point p2 = nodeMap[k].vertex;
				double distance = norm(Mat(p1), Mat(p2));
				if (distance < erosionRatio){
					sign = true;
					break;
				}
			}
			if (sign == true)
			 	preIndex = nodeMap[k].index;
			else{
				Node temp;
				temp.vertex = p1;
				vector<int> indexmap;
				indexmap.push_back(preIndex);
				nodeMap[preIndex].indexMap.push_back(nodeMap.size());
				temp.index = nodeMap.size();
				lineRefinement(nodeMap[preIndex].vertex, contours[i][j], 10);
				temp.vertex = contours[i][j];
				temp.indexMap = indexmap;
				nodeMap.push_back(temp); 
				preIndex = temp.index;
			}
		}
	}
	std::fstream myfileStream;

	std::cout << "Print map: " << nodeMap.size() << std::endl;
	for (Node x : nodeMap){
		std::cout << x.index<< "Vertex index: " << x.vertex << std::endl;
		for (auto y : x.indexMap){
			std::cout << y << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "Finish" << std::endl;
	return nodeMap;
}

Mat imageProcess(Mat src, int picIndex){
	long startTime = clock();
	//erodeAndDilate(src, 10);
	cvtColor(src, src, COLOR_BGRA2GRAY);
	src = ~src;
	Scalar rgb_min(0);
	Scalar rgb_max(250);
	threshold(src, src, 230, 255, 0);
	//inRange(src, rgb_min, rgb_max, src);

	erodeAndDilate(src, 1);
	Mat midImage, dstImage;
	cvtColor(src, dstImage, CV_GRAY2BGR);
	medianBlur(src, src, 5);

	int thresh = 100;
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	contours_poly.resize(contours.size());
	// approximate contours
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
	}

	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	std::cout << "contours size: " << contours_poly.size() << std::endl;

	vector<Node> nodeMap = contoursToMap(contours_poly,10);

	Mat drawing2 = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < nodeMap.size(); i++){
			
		for (int j = 0; j < nodeMap[i].indexMap.size(); j++){
			Point p1 = nodeMap[i].vertex;
			int index = nodeMap[i].indexMap[j];
			Point p2 = nodeMap[index].vertex;
			line(drawing2, cvPoint(p1.x, p1.y), cvPoint(p2.x, p2.y), Scalar(0, 0, 255), 1, 8);
		}
	}
	imshow("New", drawing2);

	for (int i = 0; i < contours_poly.size(); i++)
	{
		drawContours(dstImage, contours_poly, i, Scalar(0, 0, 255), 1, 8, hierarchy, 0, Point());
		drawContours(drawing, contours_poly, i, Scalar(0, 0, 255), 1, 8, hierarchy, 0, Point());
	}

	/// Show in a window
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", drawing);
	imshow("dst", dstImage);
	return drawing;
}

void openCVProcess(){
	for (int i = 9; i < 10; i++){
		String s = ("data/") + std::to_string(i) + (".jpg");
		std::cout << s << std::endl;
		Mat src = imread(s);
		imshow("src", src);
		Mat dst = imageProcess(src, i);
		String d = ("output/image/") + std::to_string(i) + ("Out.jpg");
		imwrite(d, dst);
	}

	char waitkey = waitKey(0);
	while (waitkey != ' '){
		waitKey(0);
	}
}