#include "face_detection.h"
#include "detector.h"
#include <opencv2/opencv.hpp>

FaceDetection::FaceDetection() : face_detector_(NULL) {}
FaceDetection::~FaceDetection() {}

int FaceDetection::Initial(INPARA char* model_path) {
  // Initialize trackor
  bool HOG = true;
  bool FIXEDWINDOW = true;
  bool MULTISCALE = true;
  bool SILENT = true;
  bool LAB = true;
 
  has_mask_cache_ = false;
  score_cache_ = 0.0f;
  track_id_ = 0;
  track_thresh_ = 0.2f;
  max_thresh_ = 100000.f;
  cnt_ = 0;

  // Initialize face detector
  face_detector_ = new Detector;
  std::string model_path_s = model_path;
  std::string model_path_param = model_path + std::string("/faceMask.param");
  std::string model_path_bin= model_path + std::string("/faceMask.bin");
  // printf("INFO:%s,%s\n",model_path_param.c_str(),model_path_bin.c_str());
  return ((Detector*)face_detector_)->Init(model_path_param, model_path_bin);
}

int FaceDetection::FaceDetectionConfig(
    INPARA int& min_face,
    INPARA int& max_face,
    INPARA float& threshold,
    INPARA float& scale,
    INPARA int& step_x,
    INPARA int& step_y) {
//   ((Detector*)face_detector_)->SetThreshold(threshold);
  return 0;
}

int FaceDetection::SetPara(
   const char* param, float value) {
  if (strcmp(param, "detectThreshold") == 0) {
    // ((MaskFaceDetectionCore*)face_detector_)->SetThreshold(value);
  } if (strcmp(param, "trackThreshold") == 0) {
    track_thresh_ = value;
  } if (strcmp(param, "detectInterval") == 0) {
    max_thresh_ = int(value);
  } else {
    return -1;
  }
  return 0;
}

int FaceDetection::DetectionMaxFace(
    INPARA byte* bgr_buf,
    INPARA int& width,
    INPARA int& height,
    OUTPUT FaceRect* face_rect,
    OUTPUT float* score,
    OUTPUT bool* has_mask) {
  cv::Mat image(height, width, CV_8UC3, (void*)bgr_buf);
  
  std::vector<bbox> face_infos;

  int ret =((Detector*)face_detector_)->Detect(image, face_infos);

  if (face_infos.size() <= 0) {
    return -2;
  }
  face_rect->width = 0;
  face_rect->height = 0;
  for (int i = 0; i < face_infos.size(); ++i) {
    if ((face_infos[i].x2 - face_infos[i].x1)* (face_infos[i].y2 - face_infos[i].y1) >
        face_rect->width * face_rect->height) {
      face_rect->width = face_infos[i].x2 - face_infos[i].x1;
      face_rect->height = face_infos[i].y2 - face_infos[i].y1;
      face_rect->x = face_infos[i].x1;
      face_rect->y = face_infos[i].y1;
      *score = face_infos[i].face_s;
      if(face_infos[i].mask_s > 0.6){
          *has_mask = true;
      }else{
          *has_mask = false;
      }
    }
  }

  if (face_rect->x < 0 || face_rect->y < 0 || face_rect->width+face_rect->x >= image.cols
    || face_rect->height+face_rect->y >= image.rows) {
    return -2;
  }

  return 0;
}

int FaceDetection::DetectionMaxFace(
    INPARA byte* bgr_buf,
    INPARA int& width,
    INPARA int& height,
    OUTPUT FaceRect* face_rect,
    OUTPUT float* score,
    OUTPUT bool* has_mask,
    OUTPUT int* track_id) {
  cv::Mat frame(height, width, CV_8UC3, (void*)bgr_buf);

  int ret = DetectionMaxFace(frame.data, frame.cols, frame.rows, face_rect, score, has_mask);
  if (ret != 0) {
    if(rect_cache_2_.size()>0){
        rect_cache_2_.erase(rect_cache_2_.begin());
    }
    return ret;
  }

  if (face_rect->x < 0 || face_rect->y < 0 || face_rect->width < 0 || face_rect->height < 0  
    || face_rect->width+face_rect->x >= frame.cols
    || face_rect->height+face_rect->y >= frame.rows) {
      if(rect_cache_2_.size()>0){
        rect_cache_2_.erase(rect_cache_2_.begin());
      }
      return -2;
  }

  bool pass = false;
  if (rect_cache_2_.size() > 0) {
    // Judge track ID
    int length = rect_cache_2_.size();
    for(int i=0; i<length; i++){
      cv::Rect rect_cache_ = rect_cache_2_[i];
      if (JaccardOverlap(cv::Rect(face_rect->x, face_rect->y,
        face_rect->width, face_rect->height) , rect_cache_) > track_thresh_) {
        *track_id = track_id_;  // if current bbox close to last result bbox
        pass = true;
      } 

      // cache results
      has_mask_cache_ = *has_mask;
      score_cache_ = *score;
      rect_cache_.x = face_rect->x;
      rect_cache_.y = face_rect->y;
      rect_cache_.width = face_rect->width;
      rect_cache_.height = face_rect->height;

      // remove first cache box
      if(rect_cache_2_.size() == 3){
        rect_cache_2_.erase(rect_cache_2_.begin());
      }
      // add new box
      rect_cache_2_.push_back(rect_cache_);

      // pass return
      if(pass){
        return ret;
      }
    }

    if(!pass){
        *track_id = ++track_id_;
    }
  }else{

    *track_id = ++track_id_;
    cv::Rect rect_cache_;
    rect_cache_.x = face_rect->x;
    rect_cache_.y = face_rect->y;
    rect_cache_.width = face_rect->width;
    rect_cache_.height = face_rect->height;
    rect_cache_2_.push_back(rect_cache_);
  }

  if (track_id_ == max_thresh_) {
    track_id_ = 0;
  }

  return 0;
}

const int FaceDetection::GetLandmarkLength() const {
  return 5;
}

int FaceDetection::Release() {
  return 0;
}