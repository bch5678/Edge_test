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
int xx = 210,yy = 120;

ofstream file1("./1circle.txt", ios::app);

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
	GaussianBlur(rsz, rsz, Size(3, 3), 8, 1, 4);
	//Canny(rsz, rsz, 125, 350);
	//resize(rsz, rsz, Size(450, 200),0,0, INTER_NEAREST);

	//imshow("rsz", rsz);

	Mat binary_img, dst_img;
	//src_img = imread("test2.png", IMREAD_GRAYSCALE);//灰度图读入
	threshold(rsz, binary_img, 80, 255, THRESH_BINARY);

	imshow("trs", binary_img);
	//imwrite("trs.bmp", binary_img);
//可以根据实际需求选择使用的滤波进行降噪
	GaussianBlur(binary_img, binary_img, Size(11, 11), 8,1, 4);
	//boxFilter(binary_img, binary_img,-1, Size(3, 3));
	//blur(binary_img, binary_img, Size(3, 3));
	vector<vector<Point>> contours;
	vector<Vec4i> hireachy;
	// 形态学操作    
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	// 构建形态学操作的结构元    
	morphologyEx(binary_img, binary_img, MORPH_CLOSE, kernel, Point(-1, -1));
	//闭操作    
	kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	// 构建形态学操作的结构元
	morphologyEx(binary_img, binary_img, MORPH_OPEN, kernel, Point(-1, -1));
	//开操作    
	//imshow("开操作", dst_img);
	findContours(binary_img, contours, hireachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
	//imshow("bf", binary_img);
	//imwrite("bf.bmp", binary_img);
	Mat result_img = rsz.clone();
	cvtColor(result_img, result_img, COLOR_GRAY2BGR);
	//灰度图转为彩色图   
	int MinR = 10; int MaxR = 300;
	double radius=1;
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

		if (area>100 ) {
			RotatedRect rect = fitEllipse(contours[i]);
			double arc_length = arcLength(contours[i], true);
			radius = ((arc_length / (2 * PI))+radius)/1.4;
			if (radius < 5) continue;
			//circle(src, Point(rect.center.x + xx, rect.center.y + yy), radius, Scalar(0, 255, 0), 1, 8, 0);
			circle(src, Point(rect.center.x + xx, rect.center.y + yy), 2, Scalar(0, 255, 0), -1, 8, 0);

			file1 << "num:" << num << "	" << rect.center.x+xx << "	" << rect.center.y+yy << "	" << endl;

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
	string out_videofile = "./2out_ld.avi";
	//Mat mode = imread("mode.png", IMREAD_GRAYSCALE);
	int num = 0;
	while (true)
	{
		//std::cout << "thresh: ";
		//std::cin >> contourthresh1 >> contourthresh2;//对于media2，最佳阈值为70（检测气泡）；最佳阈值为85，检测microbeads
		filename = "./1.mp4";
		//contourthresh = 120;
		//cout << filename << endl;
		if (filename.size() == 0) break;

		try {
			string const file_ext = filename.substr(filename.find_last_of(".") + 1);
			string const protocol = filename.substr(0, 7);
			if (file_ext == "avi" || file_ext == "mp4" || file_ext == "mjpg" || file_ext == "mov" || 	// video file
				protocol == "rtmp://" || protocol == "rtsp://" || protocol == "http://" || protocol == "https:/")	// video network stream
			{
				cout << "input successfully" << endl;
				Mat cap_frame, cur_frame, det_frame, write_frame, flur_frame, dst;
				Mat cur;
				atomic<bool> consumed, videowrite_ready;
				consumed = true;
				//vector<Vec2f> lines;//存储霍夫变换后的直线
				videowrite_ready = true;
				thread t_detect, t_cap, t_videowrite;
				mutex mtx;
				condition_variable cv_detected, cv_pre_tracked;
				chrono::steady_clock::time_point steady_start, steady_end;//计时
				VideoCapture cap(filename);
				cap >> cur_frame;//读入视频
				int const video_fps = cap.get(CAP_PROP_FPS);
				Size const frame_size = cur_frame.size();
				VideoWriter output_video;//处理结果
				if (save_output_videofile)
					output_video = VideoWriter(out_videofile, VideoWriter::fourcc('X', 'V', 'I', 'D'), video_fps, frame_size, true);

				while (!cur_frame.empty())
					//while (true)
				{
					//cap >> cur_frame;//读入视频
					// always sync
					if (t_cap.joinable()) {
						t_cap.join();
						//++fps_cap_counter;
						cur_frame = cap_frame.clone();
						//imshow("winn", cur_frame);
						//int key = cv::waitKey(3);
					}
					t_cap = std::thread([&]() { cap >> cap_frame; });//读取视频中的每帧图像
					cout << "1" << endl;
					//if (!cur_frame.empty())
					//{

					//}
					
					//cout << "2" << endl;
					if (!cur_frame.empty()) {
						num = num + 1;
						
						cur = on_ThreshChange(cur_frame, num);
						//imshow("src1", flur_frame);
						//将原图像转换为二值图像  
						//cv::threshold(flur_frame, flur_frame, 128, 1, cv::THRESH_BINARY);

						//二值图转化成灰度图，并绘制找到的点
						//dst = dst * 255;
						//flur_frame = flur_frame * 255;
						//显示图像  
						/*cv::namedWindow("src1", CV_WINDOW_AUTOSIZE);
						cv::namedWindow("dst1", CV_WINDOW_AUTOSIZE);
						cv::imshow("src1", flur_frame);
						cv::imshow("dst1", dst);*/
						//cur_frame = houghlinee(cur_frame);

						//char stringnum[10];
						//sprintf_s(stringnum, "%d", num);//保留四位小数
						//string timeString("frame:");
						//timeString += stringnum;
						//putText(cur_frame, timeString, Point(60, 60), CV_FONT_HERSHEY_SIMPLEX, 2, Scalar(0), 2, 8);

						steady_end = std::chrono::steady_clock::now();
						auto spenttime = std::chrono::duration<double>(steady_end - steady_start).count();
						cout << "spent time" << spenttime << endl;
						imshow("window name", cur);
						//if (num % 10 == 0)
							//system("cls");
					}
					//imshow("window name", cur_frame);
					//imshow("window namqe", cap_frame);

					int key = cv::waitKey(3);
					//if (key == 27) { exit_flag = true; break; }

					if (output_video.isOpened() && videowrite_ready) {
						if (t_videowrite.joinable()) t_videowrite.join();
						//write_frame = dst.clone();
						//write_frame = cur_frame.clone();
						//write_frame = cur.clone();
						//imshow("write", write_frame);
						videowrite_ready = false;
						t_videowrite = std::thread([&]() {
							output_video << cur; videowrite_ready = true;
						});
					}
				}
				if (t_cap.joinable()) t_cap.join();
				if (t_detect.joinable()) t_detect.join();
				if (t_videowrite.joinable()) t_videowrite.join();
				std::cout << "Video ended \n";
				break;
			}
		}
		catch (std::exception &e) { std::cerr << "exception: " << e.what() << "\n"; getchar(); }
		catch (...) { std::cerr << "unknown exception \n"; getchar(); }
		filename.clear();
	}
	 	

	waitKey(0);

}


