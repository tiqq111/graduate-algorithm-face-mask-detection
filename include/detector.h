
#ifndef FACE_MASK_DETECTOR_H
#define FACE_MASK_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <stack>
#include "net.h"
#include <chrono>

struct bbox
{
    float x1;
    float y1;
    float x2;
    float y2;
    float face_s;
    float mask_s;
    // Point point[5];
};

struct box
{
    float cx;
    float cy;
    float sx;
    float sy;
};

class Detector
{

public:
    Detector();

    int Init(const std::string &model_param, const std::string &model_bin);

    Detector(const std::string &model_param, const std::string &model_bin);

    inline void Release();

    void nms(std::vector<bbox> &input_boxes, float NMS_THRESH);

    int Detect(const cv::Mat &bgr, std::vector<bbox> &boxes);

    // void create_anchor(std::vector<box> &anchor, int w, int h);

    void create_anchor_retinaface(std::vector<box> &anchor, int w, int h);

    inline void SetDefaultParams();

    static inline bool cmp(bbox a, bbox b);

    ~Detector();

public:
    float _nms;
    float _threshold;
    float _mean_val[3];

    ncnn::Net *Net;
};
#endif //
