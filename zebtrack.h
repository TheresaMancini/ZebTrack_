#ifndef ZEBTRACK_H
#define ZEBTRACK_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <processamento.h>

namespace Ui {
class zebtrack;
}

class zebtrack : public QMainWindow
{
    Q_OBJECT

public:
    explicit zebtrack(QWidget *parent = 0);
    ~zebtrack();
    QImage mat2qimage(cv::Mat mat);
    void calcfundo(cv::VideoCapture& vid, cv::Mat& image, int frameSkip);
    void tinicial(cv::VideoCapture& vid, int num,cv:: Mat& image);
    void tfinal(cv::VideoCapture& vid, int num,cv:: Mat& image);
    void setimage(cv::Mat& image);
    void calcframes();
    void popup(QString String);
    void set_image_inicial(cv::Mat& image);
    void set_image_final(cv::Mat& image);
    void passar_video(int &atual);
    cv::Mat calculaForeground(cv::Mat &frame, cv::Mat &fundo);
    bool excluiu=false;
    char area;
    QPolygon inter,process,exclus;
    std::vector<QPolygon> areas_inter, areas_exclus;
    Process processar;


private slots:

    void on_openVideo_clicked();

    void mudaram_tamanho();

    void on_initialSlider_valueChanged(int value);

    void on_FinalSlider_valueChanged(int value);

    void on_frameBack_clicked();

    void on_frameNext_clicked();

    void on_area_de_interesse_clicked();

    void on_area_processamento_clicked();

    void on_area_exclus_clicked();

    void on_excluir_area_clicked();

    void salvar_area();//acrescentou agora

    void on_checkBox_clicked();

    void on_recalc_fundo_clicked();

    void on_SampleRate_valueChanged(int arg1);

    void on_checkBox_2_clicked();

    void on_threshold_box_valueChanged(int arg1);

    void on_inicio_process_clicked();

    void on_pausa_process_clicked();

    void on_back_process_clicked();

    void on_next_process_clicked();

    void on_rate_Box_valueChanged(int arg1);

private:
    QString s,String;
    cv::VideoCapture video;
    cv::Mat image, d, fundo_m_cor, fundo_m_pb, var_cor, var_pb, background;
    int wim, him, wlabel, hlabel,frame_ini,frame_fim,maxframe,skip,th,frameatual=0,rate;
    bool pause=false,cor=false,media=true;
//    vector<cv::Rect> boundRect;
//    vector<cv::Point2f> cm;
    Ui::zebtrack *ui;

signals:
    void desenhar();
    void exclui();
};

#endif // ZEBTRACK_H
