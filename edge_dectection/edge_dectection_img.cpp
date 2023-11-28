#include <iostream>
#include <vector>
#include <fstream>       //提供文件头文件
#include <algorithm>
#include <stdlib.h>//C++输出精度控制需要
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <opencv2/videoio/videoio.hpp>
#include <atomic> 
#include <mutex> 
#include <thread> 
#include <condition_variable>

using namespace cv;
using namespace std;
float PI = 3.1415926535897932384;
string fnm = "./2cout/";
int frame_number=40;
ofstream file1("./2cout/circle.txt", ios::app);

Mat on_ThreshChange(Mat src, int num) {
	char mk;
	Mat mid, dst, rsz;//mat类型的中间变量和目标图像  		
	dst = src.clone();//原图像深拷贝到目标图像  		
	cvtColor(dst, mid, COLOR_BGR2GRAY); //转化为灰度图像
	//Ptr<CLAHE> clahe = createCLAHE(40,Size(8,8));
	//clahe->apply(mid, mid);
	//equalizeHist(mid, mid);
	rsz = mid;
	//rsz = mid(Rect(xx, yy, 260, 200));
	//GaussianBlur(rsz, rsz, Size(3, 3), 8, 1, 4);
	//Canny(rsz, rsz, 125, 350);
	//resize(rsz, rsz, Size(450, 200),0,0, INTER_NEAREST);

	//imshow("rsz", rsz);

	Mat binary_img, dst_img;
	//src_img = imread("test2.png", IMREAD_GRAYSCALE);//灰度图读入
	threshold(rsz, binary_img, 80, 255, THRESH_BINARY);

	imshow("trs", binary_img);
	//imwrite("trs.bmp", binary_img);
//可以根据实际需求选择使用的滤波进行降噪
	//GaussianBlur(binary_img, binary_img, Size(11, 11), 8, 1, 4);
	//boxFilter(binary_img, binary_img,-1, Size(3, 3));
	//blur(binary_img, binary_img, Size(3, 3));
	vector<vector<Point>> contours;
	vector<Vec4i> hireachy;
	// 形态学操作    
	/*Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	// 构建形态学操作的结构元    
	morphologyEx(binary_img, binary_img, MORPH_CLOSE, kernel, Point(-1, -1));
	//闭操作    
	kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	// 构建形态学操作的结构元
	morphologyEx(binary_img, binary_img, MORPH_OPEN, kernel, Point(-1, -1));
	//开操作    
	//imshow("开操作", dst_img);*/
	findContours(binary_img, contours, hireachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
	//imshow("bf", binary_img);
	//imwrite("bf.bmp", binary_img);
	Mat result_img = rsz.clone();
	cvtColor(result_img, result_img, COLOR_GRAY2BGR);
	//灰度图转为彩色图   
	int MinR = 10; int MaxR = 300;
	double radius = 1;
	for (int i = 0; i < hireachy.size(); i++)
	{
		//cout << contours[1] << endl;
		//        drawContours(src_img, contours, i, Scalar(0, 0, 255), -1, 8, Mat(), 0, Point());
		if (contours[i].size() < 5)continue;
		double area = contourArea(contours[i]);

		/*if (area < 300&& area>40) {
			RotatedRect rect = fitEllipse(contours[i]);
			double arc_length = arcLength(contours[i], true);
			double radius = arc_length / (2 * PI);
			circle(result_img, rect.center, radius, Scalar(0, 255, 255), 1, 8, 0);
			circle(result_img, rect.center, 2, Scalar(0, 255, 0), -1, 8, 0);
			continue;
		}*/
		if (area < 5000) continue;
		if (area > 5000) {
			RotatedRect rect = fitEllipse(contours[i]);
			double arc_length = arcLength(contours[i], true);
			radius = ((arc_length / (2 * PI)) + radius) / 1.4;
			if (radius < 5) continue;
			//circle(src, Point(rect.center.x + xx, rect.center.y + yy), radius, Scalar(0, 255, 0), 1, 8, 0);
			circle(src, Point(rect.center.x, rect.center.y), 2, Scalar(0, 255, 0), -1, 8, 0);

			file1 << rect.center.x << "	" << rect.center.y << "	" << endl;
			string fn = fnm + to_string(num) + ".txt";
			ofstream file2(fn, ios::app);
			vector<Point>::iterator it1;
			for (it1 = contours[i].begin(); it1 != contours[i].end(); it1++) {
				float xx = it1->x, yy = it1->y;
				file2 << xx << "\t" << yy << endl;
				
			}
			file2.close();
			continue;
		}
		//if (area > 2000) continue;



		//float ratio = float(rect.size.width) / float(rect.size.height);
		//drawContours(src, contours, i, Scalar(0, 255, 0), 1, 8, hireachy,214783647,Point(xx,yy));
		//RotatedRect rect = fitEllipse(contours[i]);
		//circle(src, Point(rect.center.x + xx, rect.center.y + yy), 2, Scalar(0, 255, 0), -1, 8, 0);
		//cout << "contours:" << i << endl;
	}
	//waitKey();
	return src;
}


int main()
{
	//int contourthresh1, contourthresh2; 	
	string filename;
	bool const save_output_videofile = true;//视频存储标志位 	
	string out_videofile = "./1out_ld.avi";
	//Mat mode = imread("mode.png", IMREAD_GRAYSCALE);
	int num = 0;
	while (true)
	{
		num++;
		filename = fnm + to_string(num) + ".tif";
		Mat img = imread(filename);
		on_ThreshChange(img, num);
		filename.clear();
		if (num >= frame_number)
			break;
	}


	waitKey(27);

}


