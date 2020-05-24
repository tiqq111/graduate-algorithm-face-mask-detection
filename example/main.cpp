#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <vector>

#include <iostream>
#include "face_mask_detection.h"

using namespace std;

void readTxt(std::string file, std::vector<std::string> &image_pathes)
{
  ifstream infile;
  infile.open(file.data());

  std::string s;
  while (getline(infile, s))
  {
    image_pathes.push_back(s);
  }
  infile.close();
}

void detect(FaceMaskDetection detector, cv::Mat &img)
{
  std::vector<FaceRect> faceInfos;
  detector.DetectFaceMask(img.data, img.cols, img.rows, &faceInfos);

  for (int i = 0; i < faceInfos.size(); i++)
  {
    float width = faceInfos[i].x2 - faceInfos[i].x1;
    float height = faceInfos[i].y2 - faceInfos[i].y1;
    cv::Rect rect(faceInfos[i].x1, faceInfos[i].y1, width, height);
    cv::rectangle(img, rect, cv::Scalar(0, 0, 255), 1, 8, 0);
    char test[80];
    float face_score = faceInfos[i].face_s;
    float mask_score = faceInfos[i].mask_s;
    sprintf(test, "%f", face_score);
    printf("%f\n", face_score);
    printf("mask_score:%f\n", mask_score);

    char test2[80];
    sprintf(test2, "%f", mask_score);

    cv::putText(
        img,
        test,
        cv::Size((faceInfos[i].x1), faceInfos[i].y2),
        cv::FONT_HERSHEY_COMPLEX,
        0.4,
        cv::Scalar(255, 0, 0));
    cv::putText(
        img,
        test2,
        cv::Size((faceInfos[i].x1), faceInfos[i].y2 + 15),
        cv::FONT_HERSHEY_COMPLEX,
        0.4,
        cv::Scalar(255, 0, 0));
  }
}

int detectFromTxt(std::string txt_path)
{
  // slim or RFB
  FaceMaskDetection detector;
  // retinaface
  detector.Initial("../model/");
  std::vector<std::string> image_pathes;
  readTxt(txt_path, image_pathes);

  for (int i = 0; i < image_pathes.size(); i++)
  {
    int pos = image_pathes[i].find("F");
    std::string image_name =
        image_pathes[i].substr(pos + 1, image_pathes[i].size());
    printf("image_name:%s\n", image_name.c_str());
    std::string image_path = std::string(image_pathes[i].c_str());
    // image_path = "../data/123.jpg";
    printf("image_path:%s\n", image_path.c_str());
    cv::Mat img = cv::imread(image_path.c_str());
    printf("- %d,%d\n", img.cols, img.rows);

    detect(detector, img);

    cv::imshow("cap", img);
    cv::waitKey(0);
    cv::imwrite(std::string("./detect_result_txt/") + image_name.c_str(), img);
  }
}

int detetFromVideo(std::string videoPath)
{
  cv::VideoCapture capture;
  capture.open(videoPath);

  // slim or RFB
  FaceMaskDetection detector;
  // retinaface
  detector.Initial("../model/");
  int index = 1;
  while (1)
  {
    printf("---------------------\n");
    cv::Mat img;
    capture >> img;
    printf("(%d,%d)\n", img.cols, img.rows);

    if (!img.data || img.empty())
    {
      fprintf(stderr, "cv::imread %s failed\n");
      return -1;
    }

    detect(detector, img);

    // printf(" - index:%d\n",index);
    cv::imshow("cap", img);
    cv::waitKey(0);
    cv::imwrite(
        std::string("./detect_result_video/") + std::to_string(index) + ".jpg",
        img);
    index += 1;
  }
}

int main(int argc, char **argv)
{
  // detetFromVideo("../data/123.mp4");
  detectFromTxt("../data/imageList.txt");
  return 0;
}
