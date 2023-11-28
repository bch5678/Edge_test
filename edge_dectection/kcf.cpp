#include "img2video.hpp"

using namespace std;
using namespace cv;
img2video i2v;
string video,output_img;
void img2video_initial(Mat& vd){
    cout<<"Parameters (fps):";
    cin>>i2v.para[0]; //frame
    //i2v.para[1]=vd.cols;
    //i2v.para[2]=vd.rows;
    i2v.path_save=output_img;
    i2v.iscol=1;
}

int main() {
    // declares all required variables
    //! [vars]
    Rect roi;
    Mat frame;
    Mat frame_tmp;
    //! [vars]
    cout<<"The input video dir:";
    cin >> video;
    cout<<"The output video dir:";
    cin>>output_img;


    // create a tracker object
    //Ptr<Tracker> tracker = Tracker::create("KCF");
    int tracker_class;
    cout<<"The class of tracker:(1. KCF, 2.CSRT, 3.MIL, 4.GOTURN)";
    cin >> tracker_class;
    Ptr<Tracker> tracker;
    if(tracker_class==1){
        tracker = TrackerKCF::create();
    }
    else if (tracker_class==2){
        tracker = TrackerCSRT::create();
    }
    else if (tracker_class==3){
        tracker = TrackerMIL::create();
    }
    else if (tracker_class==4){
        tracker = TrackerGOTURN::create();
    }
    else{
        cout<<"Wrong! Set as default."<<endl;
        tracker = TrackerKCF::create();
    }
    //! [create]

    // set input video
    //! [setvideo]

    VideoCapture cap(video);
    //! [setvideo]

    // get bounding box
    //! [getframe]
    cout << "press c to leap current Image" << endl;
    cout << "press q to slect current Image" << endl;
    cout << "press empty key to start track RIO Object" << endl;

    cap >> frame;
    img2video_initial(frame);
    while (1)
    {
        char key = waitKey(1);
        if (key == 'c')  // 按c键跳帧
        {
            cap >> frame;
        }
        if (key == 'q')  // 按q键退出跳帧
        {
            break;
        }
        namedWindow("first",0);
        imshow("first", frame);

    }
    cv::destroyWindow("first");
    //! [getframe]
    //! [selectroi]选择目标roi以GUI的形式
    namedWindow("tracker",0);
    roi = selectROI("tracker", frame);
    //! [selectroi]

    //quit if ROI was not selected
    if (roi.width == 0 || roi.height == 0)
        return 0;

    // initialize the tracker
    //! [init]
    tracker->init(frame, roi);
    //! [init]
    i2v.para[1]=roi.width;
    i2v.para[2]=roi.height;
    // perform the tracking process
    printf("Start the tracking process\n");
    for (;; ) {
        // get frame from the video
        cap >> frame;

        // stop the program if no more images
        if (frame.rows == 0 || frame.cols == 0){
            i2v.endcap();
            break;
        }


        // update the tracking result
        //! [update]
        tracker->update(frame, roi);
        //! [update]

        //! [visualization]
        // draw the tracked object
        //rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);
        cout<<roi.x<<"  "<<roi.y<<"  "<<roi.height<<"  "<<roi.width<<endl;
        // show image with the tracked object
        //imshow("tracker", frame);
        //! [visualization]
        //quit on ESC button
        Mat cropimg= frame(roi);
        //imshow("cp",cropimg);
        //resize(frame,frame,Size(i2v.para[1], i2v.para[2]),0,0);
        //i2v.out=frame;
        i2v.out=cropimg;
        i2v.func();
        waitKey(33);
    }

    return 0;
}
