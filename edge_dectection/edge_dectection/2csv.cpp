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

using namespace std;
string path = "./5out/";
string center = "./5out/circle.txt";
int ini =1, endi = 161;
string pathre = "./5out/all";
ofstream fileopen;

vector<float> *inputdata(string pth) {

	vector<float> *p = new vector<float>;
	ifstream infile(pth);
	float num;
	while (!infile.eof()) {
		infile >> num;
		p->push_back(num);
	}
	p->pop_back();
	return p;
}

int main() {
	int count = 0;
	fileopen.open(pathre + ".csv");
	for (int i = ini; i <= endi; i++) {
		string path_revised = path + to_string(i) + ".txt";
		cout << path_revised << endl;
		//fileopen.open(pathre+to_string(i)+".csv");
	    //fileopen<< i <<endl;
		string readpth = path + to_string(i) + ".txt";
		vector<float>* edge = inputdata(readpth);
		vector<float>* center_axis = inputdata(center);
		vector<float>::iterator it1;
		vector<float>::iterator it2;
		it1 = center_axis->begin() + count;
		float center_axis_xy[2];
		center_axis_xy[0] = *it1;
		center_axis_xy[1] = *(it1 + 1);
		count += 2;
		for (it2 = edge->begin(); it2 != edge->end(); it2 += 2) {
			float x, y;
			x = *it2;
				y = *(it2 + 1);
			fileopen << i << "," << x-center_axis_xy[0]<< "," << y-center_axis_xy[1]  << endl;
			//cout<<x<<" "<<y<<endl;
		}

	}
	fileopen.close();
	return 0;
}