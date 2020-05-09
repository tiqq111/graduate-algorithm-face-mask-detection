#include <stdio.h>
#include <algorithm>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>

#include <iostream>
#include "face_detection.h"

using namespace std;

void readTxt(std::string file,std::vector<std::string> &image_pathes){
    ifstream infile;
    infile.open(file.data());

    std::string s;
    while(getline(infile,s)){
        image_pathes.push_back(s);
    }
    infile.close();
}

int main(int argc, char** argv)
{

    string imgPath = argv[1];

    // slim or RFB
    FaceDetection detector;
    // retinaface
    detector.Initial("../model/");
    std::vector<std::string> image_pathes;
    readTxt(argv[1],image_pathes);

    for	(int i = 0; i < image_pathes.size(); i++){

        // std::vector<std::string> image_property = split(image_pathes[i],"/");
        int pos = image_pathes[i].find("/");
        std::string image_name = image_pathes[i].substr(pos+1,image_pathes[i].size());
        printf("image_name:%s,image_path:%s\n",image_name.c_str(),image_pathes[i].c_str());
        std::string image_path = std::string("/home/autobuild/wangzhen/data/1000_people/") + image_pathes[i].c_str();
        // image_path = "../data/123.jpg";
        printf("image_path:%s\n",image_path.c_str());
        printf("- %d\n", __LINE__);
        cv::Mat img = cv::imread(image_path.c_str());
        printf("- %d,%d\n", img.cols,img.rows);

        cv::Mat img_scale = img;
        float scale = 1.0;
 
        if (img.empty())
        {
            fprintf(stderr, "cv::imread %s failed\n", image_path.c_str());
            return -1;
        }
        FaceRect boxes;

        float score;
        bool has_mask;
        int track_id;
        detector.DetectionMaxFace(img_scale.data, img_scale.cols,img_scale.rows,&boxes,&score,&has_mask,&track_id);

        printf("track_id:%d\n",track_id);
        cv::Rect rect(boxes.x/scale, boxes.y/scale, boxes.width/scale ,boxes.height/scale);
        cv::rectangle(img, rect, cv::Scalar(0, 0, 255), 1, 8, 0);
        char test[80];
        sprintf(test, "%f",score);
        printf("%f\n",score);
        printf("has_mask:%d\n",has_mask);

        char trackid[80];
        sprintf(trackid, "%d",track_id);

        cv::putText(img, test, cv::Size((boxes.x/scale), boxes.y/scale), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 0));
        cv::putText(img, trackid, cv::Size((boxes.x/scale), boxes.y/scale + 15), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 0));
        cv::imwrite(std::string("./detect_result_1000/") + image_name.c_str(), img);
    }
    return 0;
}

