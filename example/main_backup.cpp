// #include <stdio.h>
// #include <algorithm>
// #include <vector>
// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/opencv.hpp>
// #include <fstream>

// #include "detector.h"

// using namespace std;

// int main(int argc, char** argv)
// {

//     string imgPath = argv[1];;
//     // if  (argc = 1)
//     // {
//     //     imgPath = "../123.jpg";
//     // }
//     // else if (argc = 2)
//     // {
//     //     imgPath = argv[1];
//     // }
//     string param = "../model/facemask.param";
//     string bin = "../model/facemask.bin";
//     const int max_side = 260;

//     // slim or RFB
//     Detector detector(param, bin);
//     // retinaface
//     // Detector detector(param, bin, true);
//     Timer timer;
//     for	(int i = 0; i < 1; i++){


//         cv::Mat img = cv::imread(imgPath.c_str());

//         // scale
//         float long_side = std::max(img.cols, img.rows);
//         float scale = max_side/long_side;
//         cv::Mat img_scale;
//         cv::Size size = cv::Size(int(img.cols*scale+0.1), int(img.rows*scale+0.1));
//         cv::resize(img, img_scale, size);
//         printf("img_resize:(%d,%d,%d)\n",img_scale.cols,img_scale.rows,img_scale.step[0]);

//         if (img.empty())
//         {
//             fprintf(stderr, "cv::imread %s failed\n", imgPath.c_str());
//             return -1;
//         }
//         std::vector<bbox> boxes;

//         timer.tic();

//         detector.Detect(img_scale, boxes);
//         timer.toc("----total timer:");

//         // draw image
//         for (int j = 0; j < boxes.size(); ++j) {
//             cv::Rect rect(boxes[j].x1/scale, boxes[j].y1/scale, boxes[j].x2/scale - boxes[j].x1/scale, boxes[j].y2/scale - boxes[j].y1/scale);
//             cv::rectangle(img, rect, cv::Scalar(0, 0, 255), 1, 8, 0);
//             char test[80];
//             sprintf(test, "%f", boxes[j].face_s);
//             printf("%f\n",boxes[j].face_s);
//             char test2[80];
//             sprintf(test2, "%f", boxes[j].mask_s);

//             cv::putText(img, test, cv::Size((boxes[j].x1/scale), boxes[j].y1/scale), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 0));
//             cv::putText(img, test2, cv::Size((boxes[j].x1/scale), boxes[j].y1/scale + 17), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 0));
          
//         }
//         cv::imwrite("test.png", img);
//     }
//     return 0;
// }

