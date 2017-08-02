#ifndef PROCESS_H
#define PROCESS_H
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
//using namespace cv;
using namespace std;

class Process
{
public:
    Process();
    cv::Mat find_contours(cv::Mat fore);
    void dilatErosion(cv::Mat &fore);
//    void calculaForeground(cv::Mat &frame, cv::Mat &fundo,int th, bool cor,bool media,cv::Mat var_pb, cv::Mat var_cor, cv::Mat foreground);
    void blobs();
    void find(cv::Mat fore,cv::Mat drawing);
//    void find(cv::Mat fore,cv::Mat nfore);//pra testar desenhar uma nova imagem só com os contornos

private:

    //Mat fore;

};

#endif // PROCESS_H
