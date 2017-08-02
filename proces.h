#ifndef PROCES_H
#define PROCES_H

#include <QImage>
#include <opencv2/opencv.hpp>


class proces
{
public:
    proces();
    void setvideo(cv::VideoCapture& v);
    void calcfundo(cv::Mat& image, int &frameSkip);
    void tinicial (int num,cv:: Mat& image);
    void tfinal(int num,cv:: Mat& image);
    void calcframes();
    cv::Mat getfundo(int &tipo);
    int getmaxframe();
    int getframefim();
    int getframeini();
    QImage mat2qimage(cv::Mat mat);

private:
    cv::VideoCapture video;
    cv::Mat image, d, fundo_m_cor, fundo_m_pb, fundo_v_cor, fundo_v_pb;
    int frame_ini,frame_fim,maxframe;
};

#endif // PROCES_H
