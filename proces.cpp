#include "proces.h"
#include <opencv2/opencv.hpp>
#include<QDebug>

using namespace cv;

proces::proces()
{

}

void proces:: setvideo(cv::VideoCapture &v){
    video=v;
}
void proces::tinicial (int num, cv::Mat &image){
    frame_ini=num;

    video.set(CV_CAP_PROP_POS_FRAMES,num);

    video.read(image);
}
void proces::tfinal(int num, cv::Mat &image){
    frame_fim=num;

    video.set(CV_CAP_PROP_POS_FRAMES,num);

    video.read(image);
}
QImage proces::mat2qimage(cv::Mat mat) {
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }

}

void proces::calcframes(){
    maxframe=(video.get(CV_CAP_PROP_FRAME_COUNT)-1);
}

int proces::getmaxframe(){
    return maxframe;
}

void proces::calcfundo(Mat &image, int &frameSkip){
    ///calculando fundo pela média
        Mat I,sum,sqsum,variance;
        int image_count=0;
        int j=frame_ini;
        while(j<frame_fim){
            video.set(CV_CAP_PROP_POS_FRAMES,j);
            video.read(I);
            if( sum.empty() ) {
                sum = Mat::zeros( I.size(),  CV_32FC(I.channels()) );
                sqsum = cv::Mat::zeros( I.size(), CV_32FC(I.channels()) );///para variancia
            }
            cv::accumulate(I,sum);
            cv::accumulateSquare(I,sqsum);
            image_count++;
            j=j+frameSkip;
        }
        ///media
        image=sum/image_count;
        image.convertTo(image, CV_8UC3);
        fundo_m_cor=image;
        cv::cvtColor(image,fundo_m_pb,CV_BGR2GRAY);
    //    backg=fundo_m_pb;
        ///variancia
        double one_by_N= 1.0/image_count;
        variance = one_by_N*sqsum-(one_by_N*one_by_N)*sum.mul(sum);
        variance.convertTo(fundo_v_cor,CV_8UC3);
        cv::cvtColor(fundo_v_cor,fundo_v_pb,CV_BGR2GRAY);
    //    backg=fundo_v_pb;
}

Mat proces::getfundo(int &tipo){
    switch(tipo){
        case 1:
            return fundo_m_cor;
            break;
        case 2:
            return fundo_m_pb;
            break;
        case 3:
            return fundo_v_cor;
            break;
        case 4:
            return fundo_v_pb;
            break;
   }
}
