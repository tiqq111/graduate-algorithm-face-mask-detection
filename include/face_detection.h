/**
 * Copyright (C) 2020 wavewisdom-bj
 *
 * @file     face_detection.h
 * @brief    Wave SDK Face detection C/C++ Interface
 *
 * @author   wangzhen (wangzhen@wavewisdom.com)
 * @version
 * @data     2020-05-07
 */
#ifndef LIB_FACE_DETECTION_H_
#define LIB_FACE_DETECTION_H_
#endif

#ifndef NULL
#define NULL 0
#endif

#define INPARA const
#define IO
#define OUTPUT
#define INARRAY

#ifndef byte
typedef unsigned char byte;
#endif

#include <opencv2/opencv.hpp>
using namespace std;

struct FaceRect {
  int x;
  int y;
  int width;
  int height;
  FaceRect(int x_, int y_, int width_, int height_)
      : x(x_), y(y_), width(width_), height(height_) {}

  FaceRect() : x(0), y(0), width(0), height(0) {}

  FaceRect(const FaceRect& other)
      : x(other.x), y(other.y), width(other.width), height(other.height) {}

  FaceRect& operator=(const FaceRect& other) {
    x = other.x;
    y = other.y;
    width = other.width;
    height = other.height;
    return *this;
  }
};

class FaceDetection {
 public:
  FaceDetection();
  ~FaceDetection();

 public:
  /**
   * @brief Initial
   * @param [in] modelPath, the path of model file
   * @return 0:success;-1:failed
   */
  int Initial(INPARA char* model_path);

  /**
   * @brief config face detection params
   * @param [in]  min_face, min face
   * @param [in]  max_face, max face
   * @param [in]  threshold, detection threshold
   * @param [in]  scale,
   * @param [in]  step_x,
   * @param [in]  step_y,
   * @return 0:success;-1:failed
   */
  int FaceDetectionConfig(
      INPARA int& min_face = 80,
      INPARA int& max_face = 1000,
      INPARA float& threshold = 0.9,
      INPARA float& scale = 0.8,
      INPARA int& step_x = 4,
      INPARA int& step_y = 4);

  /**
   * @brief detect max face
   * @param [in]  rgb, image rgb data
   * @param [in]  width, width of image
   * @param [in]  height, height of image
   * @param [out] result_buffer,only one max face result;
     detect result(face_num, score, x, y, w, h, landmark_x1, landmark_y1,...,
   landmark_xN, landmark_yN)
   * @return 0:success;-1:failed
   */
  // int DetectionMaxFace(INPARA byte* rgb, INPARA int &width, INPARA int&
  // height,
  //                      OUTPUT byte **result_buffer);

  /**
   * @brief detect max face
   * @param [in]  rgb, image rgb data
   * @param [in]  width, width of image
   * @param [in]  height, height of image
   * @param [out] face_rect,detect result(x, y, width, height)
   * @param [out] score, the score of face
   * @return 0:success;-1:failed
   */
  int DetectionMaxFace(
      INPARA byte* rgb,
      INPARA int& width,
      INPARA int& height,
      OUTPUT FaceRect* face_rect,
      OUTPUT float* score,
      OUTPUT bool* has_mask);

  int DetectionMaxFace(
      INPARA byte* rgb,
      INPARA int& width,
      INPARA int& height,
      OUTPUT FaceRect* face_rect,
      OUTPUT float* score,
      OUTPUT bool* has_mask,
      OUTPUT int* track_id);

  /**
   * @brief detect max face
   * @param [in]  rgb, image rgb data
   * @param [in]  width, width of image
   * @param [in]  height, height of image
   * @param [out] result_buffer,detect result(face_num, score, x, y, w, h,
   landmark_x1, landmark_y1,..., landmark_xN, landmark_yN) score, x, y, w, h,
   landmark_x1, landmark_y1,..., landmark_xN, landmark_yN
   * @return 0:success;-1:failed
   */
  // int Detection(INPARA byte* rgb, INPARA int &width, INPARA int& height,
  //               OUTPUT byte **result_buffer);

  // return landmarkLength
  const int GetLandmarkLength() const;

  int SetPara(const char* param, float value);

  /**
   * @brief release sources
   * @return 0:success;-1:failed
   */
  int Release();

 private:

    /** \brief Caculate overlap rate between two bboxes.
    \param bbox1 Input a bbox (Arrange: x_min, y_min, x_max, y_max).
    \param bbox2 Input a bbox (Arrange: x_min, y_min, x_max, y_max).
  */
  float JaccardOverlap(cv::Rect rect1, cv::Rect rect2) {
    float bbox1[4];
    float bbox2[4];
    bbox1[0] = rect1.x;
    bbox1[1] = rect1.y;
    bbox1[2] = rect1.width + rect1.x;
    bbox1[3] = rect1.height + rect1.y;
    bbox2[0] = rect2.x;
    bbox2[1] = rect2.y;
    bbox2[2] = rect2.width + rect2.x;
    bbox2[3] = rect2.height + rect2.y;
    if (bbox1[0] > bbox1[2] || bbox1[1] > bbox1[3] ||
        bbox2[0] > bbox2[2] || bbox2[1] > bbox2[3]) {
      return 0.0f;
    }
    float inter_x_min = std::max(bbox1[0], bbox2[0]);
    float inter_y_min = std::max(bbox1[1], bbox2[1]);
    float inter_x_max = std::min(bbox1[2], bbox2[2]);
    float inter_y_max = std::min(bbox1[3], bbox2[3]);

    float inter_region = (inter_y_max - inter_y_min) *
      (inter_x_max - inter_x_min);
    float all_region = (bbox1[2] - bbox1[0]) * (bbox1[3] - bbox1[1]) + 
      (bbox2[2] - bbox2[0]) * (bbox2[3] - bbox2[1]) - inter_region;
    return inter_region / all_region;
  }

 private:
  void* face_detector_;

  std::vector<cv::Rect> rect_cache_2_;
  float score_cache_;
  bool has_mask_cache_;

  // KCFTracker* tracker_;

  int max_thresh_;
  int cnt_;
  int track_id_;
  float track_thresh_;
};