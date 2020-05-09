#include <algorithm>
//#include "omp.h"
#include "detector.h"

Detector::Detector():
        _nms(0.4),
        _top_k(5),
        _dim_w(320),
        _threshold(0.6),
        _mean_val{104.f, 117.f, 123.f},
        Net(new ncnn::Net())
{
}

inline void Detector::Release(){
    if (Net != nullptr)
    {
        delete Net;
        Net = nullptr;
    }
}

Detector::Detector(const std::string &model_param, const std::string &model_bin):
        _nms(0.4),
        _top_k(5),
        _dim_w(320),
        _threshold(0.6),
        _mean_val{104.f, 117.f, 123.f},
        Net(new ncnn::Net())
{
    Init(model_param, model_bin);
}

int Detector::Init(const std::string &model_param, const std::string &model_bin)
{
    int ret = Net->load_param(model_param.c_str());
    ret = Net->load_model(model_bin.c_str());

    return 0;
}

int Detector::Detect(const cv::Mat& bgr, std::vector<bbox>& boxes)
{

    int im_height = bgr.rows;
    int im_width = bgr.cols;
    int dim_w = _dim_w;
    int dim_h = _dim_w * im_height * 1.0 / (im_width * 1.0);
    // printf("INFO:bgr(%d,%d),(%d,%d)\n",im_width,im_height,dim_w,dim_h);
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_RGB, bgr.cols, bgr.rows, dim_w, dim_h);
    in.substract_mean_normalize(_mean_val, 0);
    ncnn::Extractor ex = Net->create_extractor();
    ex.set_light_mode(true);
    ex.set_num_threads(4);
    ex.input(0, in);
    ncnn::Mat out, out1, out2;

    // loc
    ex.extract("511", out);

    // face classification
    ex.extract("586", out1);

    // mask classification
    ex.extract("587", out2);
    
    std::vector<box> anchor;
    create_anchor_retinaface(anchor,  dim_w, dim_h);

    std::vector<bbox> total_box;
    float *ptr = out.channel(0);
    float *ptr1 = out1.channel(0);
    float *ptr2 = out2.channel(0);

    // #pragma omp parallel for num_threads(2)'
    for (int i = 0; i < anchor.size(); ++i)
    {
        if (*(ptr1+ 1) > _threshold)
        {
            box tmp = anchor[i];
            box tmp1;
            bbox result;

            // loc and conf
            tmp1.cx = tmp.cx + *ptr * 0.1 * tmp.sx;
            tmp1.cy = tmp.cy + *(ptr+1) * 0.1 * tmp.sy;
            tmp1.sx = tmp.sx * exp(*(ptr+2) * 0.2);
            tmp1.sy = tmp.sy * exp(*(ptr+3) * 0.2);

            result.x1 = (tmp1.cx - tmp1.sx/2) * im_width;

            if (result.x1<0)
                result.x1 = 0;
            result.y1 = (tmp1.cy - tmp1.sy/2) * im_height;

            if (result.y1<0)
                result.y1 = 0;
            result.x2 = (tmp1.cx + tmp1.sx/2) * im_width;
            if (result.x2>im_width)
                result.x2 = im_width;
            result.y2 = (tmp1.cy + tmp1.sy/2)* im_height;
            if (result.y2>im_height)
                result.y2 = im_height;
            result.face_s = *(ptr1 + 1);
            result.mask_s = *(ptr2 + 1);

            total_box.push_back(result);
        }
        ptr += 4;
        ptr1 += 2;
        ptr2 += 2;

    }

    if(total_box.size() <= 0){
        return -2;
    }

    std::sort(total_box.begin(), total_box.end(), cmp);
    std::vector<bbox> top_k_box;
    for (int i=0 ; i<_top_k ; i++){
        top_k_box.push_back(total_box[i]);
    }
    nms(top_k_box, _nms);

    for (int j = 0; j < top_k_box.size(); ++j)
    {
        boxes.push_back(top_k_box[j]);
    }

    return 0;
}

inline bool Detector::cmp(bbox a, bbox b) {
    if (a.face_s > b.face_s)
        return true;
    return false;
}

inline void Detector::SetDefaultParams(){
    _nms = 0.4;
    _threshold = 0.6;
    _mean_val[0] = 104;
    _mean_val[1] = 117;
    _mean_val[2] = 123;
    Net = nullptr;

}

Detector::~Detector(){
    Release();
}

void Detector::create_anchor_retinaface(std::vector<box> &anchor, int w, int h)
{
//    anchor.reserve(num_boxes);
    anchor.clear();
    std::vector<std::vector<int> > feature_map(3), min_sizes(3);
    float steps[] = {8, 16, 32};
    for (int i = 0; i < feature_map.size(); ++i) {
        feature_map[i].push_back(ceil(h/steps[i]));
        feature_map[i].push_back(ceil(w/steps[i]));
    }
    std::vector<int> minsize1 = {8, 16};
    min_sizes[0] = minsize1;
    std::vector<int> minsize2 = {32, 64};
    min_sizes[1] = minsize2;
    std::vector<int> minsize3 = {128, 256};
    min_sizes[2] = minsize3;

    for (int k = 0; k < feature_map.size(); ++k)
    {
        std::vector<int> min_size = min_sizes[k];
        for (int i = 0; i < feature_map[k][0]; ++i)
        {
            for (int j = 0; j < feature_map[k][1]; ++j)
            {
                for (int l = 0; l < min_size.size(); ++l)
                {
                    // divide w or h is normalization
                    float s_kx = min_size[l]*1.0/w;
                    float s_ky = min_size[l]*1.0/h;
                    float cx = (j + 0.5) * steps[k]/w;
                    float cy = (i + 0.5) * steps[k]/h;
                    box axil = {cx, cy, s_kx, s_ky};
                    anchor.push_back(axil);
                }
            }
        }
    }

}

void Detector::nms(std::vector<bbox> &input_boxes, float NMS_THRESH)
{
    std::vector<float>vArea(input_boxes.size());
    for (int i = 0; i < int(input_boxes.size()); ++i)
    {
        vArea[i] = (input_boxes.at(i).x2 - input_boxes.at(i).x1 + 1)
                   * (input_boxes.at(i).y2 - input_boxes.at(i).y1 + 1);
    }
    for (int i = 0; i < int(input_boxes.size()); ++i)
    {
        for (int j = i + 1; j < int(input_boxes.size());)
        {
            float xx1 = std::max(input_boxes[i].x1, input_boxes[j].x1);
            float yy1 = std::max(input_boxes[i].y1, input_boxes[j].y1);
            float xx2 = std::min(input_boxes[i].x2, input_boxes[j].x2);
            float yy2 = std::min(input_boxes[i].y2, input_boxes[j].y2);
            float w = std::max(float(0), xx2 - xx1 + 1);
            float   h = std::max(float(0), yy2 - yy1 + 1);
            float   inter = w * h;
            float ovr = inter / (vArea[i] + vArea[j] - inter);
            if (ovr >= NMS_THRESH)
            {
                input_boxes.erase(input_boxes.begin() + j);
                vArea.erase(vArea.begin() + j);
            }
            else
            {
                j++;
            }
        }
    }
}
