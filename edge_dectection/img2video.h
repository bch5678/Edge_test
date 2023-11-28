//
// Created by dpln on 2022/4/18.
//

#ifndef KCF_IMG2VIDEO_HPP
#define KCF_IMG2VIDEO_HPP
#include <opencv2/video/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <cstring>
#include <cstdio>
#include <fstream>       //提供文件头文件
#include <algorithm>
#include <stdlib.h>//C++输出精度控制需要
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
using namespace std;
using namespace cv;

class img2video {
public:
    Mat out;
    string path_save;
    void func();
    int para[3];
    int endcap();
    int iscol;
private:
    Mat img;
    int flag=1;
    VideoWriter writer;
    int isColor = 1;
    void start();

};


void img2video::start() {
    cout << "frame_width is " << para[1] << endl;
    cout << "frame_height is " << para[2] << endl;
    cout << "frame_fps is " << para[0] << endl;
    writer = VideoWriter(path_save, VideoWriter::fourcc('X', 'V', 'I', 'D'), para[0], Size(para[1], para[2]), iscol);
    namedWindow("image to video", 0);
}

void img2video::func(){
    if(flag==1){
        start();
        flag=0;
        cout<<"Output has been finished."<<endl;

    }
    //int num = 627 + 103;//输入的图片总张数
//sprintf_s(image_name, "%d%s", ++i, ".jpeg");
//string s_image_name = path_scr + to_string(i) + ".jpeg";
    img = out;//读入图片
    if (!img.data)//判断图片调入是否成功
    {
        cout << "Could not load image file...\n" << endl;
    }
    imshow("image to video", img);
//写入
    //if(flag==0){
    //    imwrite("./tracker.png",img);
    //    flag=-1;
    // }

    writer.write(img);
}

int img2video::endcap() {
    return 1;
}
#endif //KCF_IMG2VIDEO_HPP
