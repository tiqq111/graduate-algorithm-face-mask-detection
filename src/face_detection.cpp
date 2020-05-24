#include <opencv2/opencv.hpp>
#include "detector.h"
#include "face_mask_detection.h"

FaceMaskDetection::FaceMaskDetection() : face_mask_detector_(NULL) {}
FaceMaskDetection::~FaceMaskDetection() {}

int FaceMaskDetection::Initial(INPARA char *model_path)
{
  // Initialize face detector
  face_mask_detector_ = new Detector;
  std::string model_path_s = model_path;
  std::string model_path_param =
      model_path + std::string("/faceMask.param");
  std::string model_path_bin = model_path + std::string("/faceMask.bin");

  return ((Detector *)face_mask_detector_)
      ->Init(model_path_param, model_path_bin);
}

int FaceMaskDetection::FaceDetectionConfig(
    INPARA int &min_face,
    INPARA int &max_face,
    INPARA float &threshold,
    INPARA float &scale,
    INPARA int &step_x,
    INPARA int &step_y)
{

  return 0;
}

int FaceMaskDetection::SetPara(const char *param, float value)
{
  if (strcmp(param, "detectThreshold") == 0)
  {
  }
  else
  {
    return -1;
  }
  return 0;
}

int FaceMaskDetection::DetectFaceMask(
    INPARA byte *bgr_buf,
    INPARA int &width,
    INPARA int &height,
    OUTPUT std::vector<FaceRect> *face_rects)
{
  cv::Mat image(height, width, CV_8UC3, (void *)bgr_buf);

  std::vector<bbox> face_infos;

  int ret = ((Detector *)face_mask_detector_)->Detect(image, face_infos);

  if (face_infos.size() <= 0)
  {
    return -2;
  }

  for (int i = 0; i < face_infos.size(); ++i)
  {
    FaceRect face_rect;
    face_rect.x1 = face_infos[i].x1;
    face_rect.y1 = face_infos[i].y1;
    face_rect.x2 = face_infos[i].x2;
    face_rect.y2 = face_infos[i].y2;
    face_rect.face_s = face_infos[i].face_s;
    face_rect.mask_s = face_infos[i].mask_s;

    face_rects->push_back(face_rect);
  }

  return 0;
}

int FaceMaskDetection::Release()
{
  delete[] face_mask_detector_;
  return 0;
}