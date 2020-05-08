#include <stdio.h>
#include <algorithm>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>

#include "face_detection.h"

using namespace std;

int main(int argc, char** argv)
{

    string imgPath = argv[1];;
    // if  (argc = 1)
    // {
    //     imgPath = "../123.jpg";
    // }
    // else if (argc = 2)
    // {
    //     imgPath = argv[1];
    // }
    string param = "../model/facemask.param";
    string bin = "../model/facemask.bin";
    const int max_side = 260;

    // slim or RFB
    FaceDetection detector;
    // retinaface
    // Detector detector(param, bin, true);
    detector.Initial("../model/");
    printf("INFO:init success\n");
    for	(int i = 0; i < 1; i++){


        cv::Mat img = cv::imread(imgPath.c_str());

        // scale
        float long_side = std::max(img.cols, img.rows);
        float scale = max_side/long_side;
        cv::Mat img_scale;
        cv::Size size = cv::Size(int(img.cols*scale+0.1), int(img.rows*scale+0.1));
        cv::resize(img, img_scale, size);
        printf("img_resize:(%d,%d,%d)\n",img_scale.cols,img_scale.rows,img_scale.step[0]);

        if (img.empty())
        {
            fprintf(stderr, "cv::imread %s failed\n", imgPath.c_str());
            return -1;
        }
        FaceRect boxes;


        float score;
        float has_mask;
        int track_id;
        detector.DetectionMaxFace(img_scale.data, img_scale.cols,img_scale.rows,&boxes,&score,&has_mask,&track_id);

        // draw image
        // for (int j = 0; j < boxes.size(); ++j) {
        //     cv::Rect rect(boxes[j].x/scale, boxes[j].y/scale, boxes[j].width/scale ,boxes[j].height/scale);
        //     cv::rectangle(img, rect, cv::Scalar(0, 0, 255), 1, 8, 0);
        //     char test[80];
        //     sprintf(test, "%f",score);
        //     printf("%f\n",score);
        //     char test2[80];
        //     sprintf(test2, "%f", has_mask);

        //     cv::putText(img, test, cv::Size((boxes[j].x/scale), boxes[j].y/scale), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 0));
        //     cv::putText(img, test2, cv::Size((boxes[j].x/scale), boxes[j].y/scale + 17), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 0));
          
        // }

        cv::Rect rect(boxes.x/scale, boxes.y/scale, boxes.width/scale ,boxes.height/scale);
        cv::rectangle(img, rect, cv::Scalar(0, 0, 255), 1, 8, 0);
        char test[80];
        sprintf(test, "%f",score);
        printf("%f\n",score);
        char test2[80];
        sprintf(test2, "%f", has_mask);

        cv::putText(img, test, cv::Size((boxes.x/scale), boxes.y/scale), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 0));
        cv::putText(img, test2, cv::Size((boxes.x/scale), boxes.y/scale + 17), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 0));
        cv::imwrite("test.png", img);
    }
    return 0;
}

