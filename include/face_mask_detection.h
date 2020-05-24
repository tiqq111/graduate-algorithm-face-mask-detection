/**
 *
 * @file     face_mask_detection.h
 *
 * @author   wangzhen
 * @version
 * @data     2020-05-13
 */
#ifndef LIB_FACE_MASK_DETECTION_H_
#define LIB_FACE_MASK_DETECTION_H_
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

struct FaceRect
{
  float x1;
  float y1;
  float x2;
  float y2;
  float face_s;
  float mask_s;
};

class FaceMaskDetection
{
public:
  FaceMaskDetection();
  ~FaceMaskDetection();

public:
  /**
   * @brief Initial
   * @param [in] modelPath, the path of model file
   * @return 0:success;-1:failed
   */
  int Initial(INPARA char *model_path);

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
      INPARA int &min_face = 80,
      INPARA int &max_face = 1000,
      INPARA float &threshold = 0.9,
      INPARA float &scale = 0.8,
      INPARA int &step_x = 4,
      INPARA int &step_y = 4);

  /**
   * @brief detect  face and mask
   * @param [in]  rgb, image rgb data
   * @param [in]  width, width of image
   * @param [in]  height, height of image
   * @param [out] face_rect,detect result(x, y, width,
   * height,face_score,mask_score)
   * @return 0:success;-1:failed
   */
  int DetectFaceMask(
      INPARA byte *rgb,
      INPARA int &width,
      INPARA int &height,
      OUTPUT std::vector<FaceRect> *face_rects);

  int SetPara(const char *param, float value);

  /**
   * @brief release sources
   * @return 0:success;-1:failed
   */
  int Release();

private:
  void *face_mask_detector_;
};