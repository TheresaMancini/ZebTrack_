#include "zebtrack.h"
#include "ui_zebtrack.h"
#include "labeltrack.h"
#include <QtGui>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <processamento.h>
using namespace std;
using namespace cv;
zebtrack::zebtrack(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::zebtrack)
{
    ui->setupUi(this);
    connect(ui->figura,SIGNAL(mudou_tamanho()),this,SLOT(mudaram_tamanho()));
    connect(this,SIGNAL(desenhar()),ui->figura,SLOT(desenhando())); // sinal desenhar aqui, no label desenhando com flag virando true
    connect(ui->figura,SIGNAL(desenhou()),this,SLOT(salvar_area()));// doubleclick salvar área
    connect(this,SIGNAL(exclui()),ui->figura,SLOT(area_excluiuda())); //testando
}

zebtrack::~zebtrack()
{
    delete ui;
}
void zebtrack:: salvar_area(){

      if(area=='i'){
        inter=ui->figura->poly;
        areas_inter.push_back(inter);
        QTextStream out(stdout);
        out << "x primeiro= " << areas_inter[0].point(0).x() << endl;
        if(areas_inter.size()>1)
        out << "x segundo= " << areas_inter[1].point(0).x() << endl;
        ui->figura->poly.clear();
    }
      else if(area=='p'){
        process=ui->figura->poly;
        ui->figura->poly.clear();
    }
      else if(area=='e'){
        exclus=ui->figura->poly;
        areas_exclus.push_back(exclus);
        ui->figura->poly.clear();
    }
      excluiu=false;
}

void zebtrack::popup(QString String){
    QMessageBox msgBox;
    msgBox.setText(String);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Ok:
          // Ok was clicked
          break;
      default:
          // should never be reached
          break;
    }
}

void zebtrack::calcframes(){
    maxframe=(video.get(CV_CAP_PROP_FRAME_COUNT)-1); // get number of frames
    ui->initialSlider->setRange(0,maxframe-1);
    ui->FinalSlider->setRange(0,maxframe);
}
QImage zebtrack::mat2qimage(cv::Mat mat) {
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
void zebtrack::tinicial(VideoCapture& vid, int num, Mat& image){
    frame_ini=num;

    vid.set(CV_CAP_PROP_POS_FRAMES,num);

    vid.read(image);
}
void zebtrack::tfinal(VideoCapture& vid, int num, Mat& image){
    frame_fim=num;

    vid.set(CV_CAP_PROP_POS_FRAMES,num);

    vid.read(image);
}
void zebtrack::setimage(Mat& image){
    Mat fundon;
    if(image.type()==CV_32FC1){
        image.convertTo(fundon,CV_8UC1);
    }
    else
        image.convertTo(fundon,CV_8UC3);
    // resize the label to keep aspect ratio
   wim = fundon.cols;
   him = fundon.rows;
   wlabel = ui->figura->width();
   hlabel = ui->figura->height();
   if(wim>him){
       ui->figura->resize(wlabel,int(floor(wlabel*him/wim)));

   }else{

        ui->figura->resize(int(floor(hlabel*wim/him)),hlabel);
   }
   // display on label

    ui->figura->setPixmap(QPixmap::fromImage(mat2qimage(fundon)));

}

void zebtrack::set_image_inicial(Mat& image){
    // resize the label to keep aspect ratio
    Size size(100,100);//the dst image size,e.g.100x100
    Mat dst;//dst image
    cv::resize(image,dst,size);

    ui->frame_inicial->setPixmap(QPixmap::fromImage(mat2qimage(dst)));
}
void zebtrack::set_image_final(Mat& image){
    Size size(100,100);//the dst image size,e.g.100x100
    Mat dst;//dst image
    cv::resize(image,dst,size);

    ui->frame_final->setPixmap(QPixmap::fromImage(mat2qimage(dst)));
}

void zebtrack::on_openVideo_clicked()
{

    QString nombre= QFileDialog::getOpenFileName();
        QFile file(nombre);
        if(!file.open(QIODevice::ReadOnly)){
//                    QMessageBox::critical(this, tr("Error"),
//                            tr("Could not open file"));
            return;
        }
    VideoCapture vid(nombre.toStdString());

    video=vid;

   calcframes();
   tinicial(vid,0,image);
   set_image_inicial(image);
   tfinal(vid,maxframe,image);
   set_image_final(image);
   calcfundo(vid,image,60);
   setimage(image);


   ui->initialSlider->setValue(0);
   ui->T_inicial->setText(s= QString::number(0));
   ui->FinalSlider->setValue(maxframe);
   ui->T_final->setText(s = QString::number(maxframe));
   ui->SampleRate->setValue(int(maxframe/120));
   ui->SampleRate->setMinimum(1);
   ui->SampleRate->setMaximum(maxframe-2);
   ui->threshold_box->setValue(8);
   ui->rate_Box->setValue(int(maxframe/250));
}

void zebtrack::calcfundo(cv::VideoCapture& vid, Mat& backgpb, int frameSkip){
///calculando fundo pela média

    Mat I,Ipb,sum,sqsum,variance,variancepb,sumpb,sqsumpb,backg/*,framediff,frameprev*/;
    int image_count=0;
    int j=frame_ini;
    while(j<frame_fim){
        vid.set(CV_CAP_PROP_POS_FRAMES,j);
        vid.read(I);
        I.convertTo(Ipb,CV_8UC3);
        cv::cvtColor(Ipb,Ipb,CV_BGR2GRAY);

        if( sum.empty() ) {
            sum = Mat::zeros( I.size(),  CV_32FC(I.channels()) );
            sqsum = cv::Mat::zeros( I.size(), CV_32FC(I.channels()) );///para variancia
            sumpb = Mat::zeros( Ipb.size(),  CV_32FC(Ipb.channels()) );
            sqsumpb = cv::Mat::zeros( Ipb.size(), CV_32FC(Ipb.channels()) );
            backgpb = cv::Mat::zeros( Ipb.size(), CV_32FC(Ipb.channels()) );
//            frameprev = Ipb;
        }
        cv::accumulate(I,sum);
        cv::accumulateSquare(I,sqsum);
//        cv::absdiff(Ipb,frameprev,framediff);
//        cv::accumulate(framediff,sqsumpb); //spsum avararegedii
        cv::accumulate(Ipb,sumpb);
        cv::accumulateSquare(Ipb,sqsumpb);
        image_count++;
        j=j+frameSkip;
    }
    ///media
    backg=sum/image_count; ///fundo_m_cor
    backgpb=sumpb/image_count; ///fundo_m_pb
    fundo_m_cor=backg;
    fundo_m_pb=backgpb;
//////variancia
    double one_by_N= 1.0/image_count;
    variance = one_by_N*sqsum-(one_by_N*one_by_N)*sum.mul(sum);
    variancepb = one_by_N*sqsumpb-(one_by_N*one_by_N)*sumpb.mul(sumpb);
/////// Avarage Differencer
//    variancepb=sqsumpb/image_count;

    var_cor=variance;
    var_pb=variancepb;
    cv::pow(var_pb,0.5,var_pb);
    cv::pow(var_cor,0.5,var_cor);
    for(int i=0;i<var_pb.rows;i++){
        for(int j=0;j<var_pb.cols;j++){
            if ((var_pb.at<float>(i,j))<0.5)
                var_pb.at<float>(i,j)=0.5;
        }
    }
//     QTextStream out(stdout);
    for(int i=0;i<var_cor.rows;i++){
        for(int j=0;j<var_cor.cols;j++){
            if ((var_cor.at<float>(i,j))<0.5){
                var_cor.at<float>(i,j)=0.5;
           }
        }
    }

//    QTextStream out(stdout);
//    for (int x=300;x<401;x++){
//        for(int y=200; y<251;y++){
//      out <<var_pb.at<float>(x,y) <<endl;
//        }
//     }

    String="Fundo Calculado!";
    popup(String);
    background=fundo_m_pb;
}

Mat zebtrack::calculaForeground(cv::Mat &frame, cv::Mat &fundo){ /// calcula a diferença entre o fundo e o frame atual
////ok
    Mat auxFrame, auxFundo, auxFore, foreGray, foreground;
    frame.convertTo(auxFrame, CV_8UC3);
    if (!cor){
        fundo.convertTo(auxFundo, CV_32FC1);
        cv::cvtColor(auxFrame,auxFrame,CV_BGR2GRAY);
        auxFrame.convertTo(auxFrame,CV_32FC1);
        absdiff(auxFundo,auxFrame,auxFore);
//        auxFore.convertTo(auxFore, CV_8UC1);
        blur (auxFore,foreGray,Size(3,3));
        // apply threshold to foreground image
        if(!media){
            Mat Ilow;
            var_pb.convertTo(var_pb, CV_32FC1);
//            QTextStream out(stdout);

            Ilow=auxFundo-((var_pb)*th);
            Mat Ihi=auxFundo+((var_pb)*th);
//            for (int x=300;x<401;x++){
//                for(int y=200; y<251;y++){
//              out <<"Ihi = " <<var_pb.at<float>(x,y) <<endl;
//                }
//             }
            cv::inRange(auxFrame,Ilow,Ihi,foreground);
            foreground=255-foreground;
        }
        else if(media){
              threshold(foreGray,foreground,th,255,0);
        }

    }
    else{
        Mat auxR, auxG, auxB;

        Mat canalR = Mat::zeros(frame.rows,frame.cols, CV_32FC1);
        Mat canalG = Mat::zeros(frame.rows,frame.cols, CV_32FC1);
        Mat canalB = Mat::zeros(frame.rows,frame.cols, CV_32FC1);
        Mat canalRf = Mat::zeros(fundo.rows,fundo.cols, CV_32FC1);
        Mat canalGf = Mat::zeros(fundo.rows,fundo.cols, CV_32FC1);
        Mat canalBf = Mat::zeros(fundo.rows,fundo.cols, CV_32FC1);
        vector<Mat> bgr_planes(3);
        vector<Mat> bgr_planesf(3);
        fundo.convertTo(auxFundo,CV_32FC3);
        auxFrame.convertTo(auxFrame,CV_32FC3);
        split(auxFrame, bgr_planes);
        split(auxFundo, bgr_planesf);

        canalR += bgr_planes[2];
        canalRf += bgr_planesf[2];
        canalG += bgr_planes[1];
        canalGf += bgr_planesf[1];
        canalB += bgr_planes[0];
        canalBf += bgr_planesf[0];

        absdiff(canalR,canalRf,auxR);
        absdiff(canalB,canalBf,auxB);
        absdiff(canalG,canalGf,auxG);

       if(!media){
            QTextStream out(stdout);
            Mat auxvarR = Mat::zeros(frame.rows,frame.cols, CV_32FC1);
            Mat auxvarG = Mat::zeros(frame.rows,frame.cols, CV_32FC1);
            Mat auxvarB = Mat::zeros(frame.rows,frame.cols, CV_32FC1);
           Mat Ilow,foreG,foreB,foreR;
           var_cor.convertTo(var_cor, CV_32FC3);
           vector<Mat> bgr_var(3);
           split(var_cor, bgr_var);


           auxvarR += bgr_var[2];
           auxvarG += bgr_var[1];
           auxvarB += bgr_var[0];

           Ilow=canalRf-((auxvarR)*th);
           Mat Ihi=canalRf+((auxvarR)*th);
           cv::inRange(auxR,Ilow,Ihi,foreR);
//           foreR=255-foreR;
           Ilow=canalBf-((auxvarB)*th);
           Ihi=canalBf+((auxvarB)*th);
           cv::inRange(auxB,Ilow,Ihi,foreB);
//           foreB=255-foreB;
           Ilow=canalGf-((auxvarG)*th);
           Ihi=canalGf+((auxvarG)*th);
           cv::inRange(auxG,Ilow,Ihi,foreG);
//           foreG=255-foreG;

           bitwise_or(foreR,foreB,auxFore);
           bitwise_or(auxFore,foreG,foreground);
//            auxFore=cv::min(foreR,foreB);
//            foreground=cv::min(auxFore,foreG);

//           foreground=255-foreground;


       }
       else if(media){
           threshold(auxR,auxR,th,255,0);
           threshold(auxG,auxG,th,255,0);
           threshold(auxB,auxB,th,255,0);

           bitwise_or(auxR,auxG,auxFore);
           bitwise_or(auxFore,auxB,foreground);
       }

    }
    return foreground;

}


void zebtrack::mudaram_tamanho(){
    wlabel = ui->figura->width();
    hlabel = ui->figura->height();
    if(wim>him){
        ui->figura->resize(wlabel,int(floor(wlabel*him/wim)));

    }else{
       ui->figura->resize(int(floor(hlabel*wim/him)),hlabel);
    }
}

void zebtrack::on_initialSlider_valueChanged(int value)
{
    if (ui->figura->pixmap()==0){

    }
    else if(value>=maxframe){
        ui->initialSlider->setValue(maxframe-1);
        ui->T_inicial->setText (s = QString::number(maxframe-1));
        tinicial(video,(maxframe-1),image);
        set_image_inicial(image);
    }
    else{
        ui->T_inicial->setText (s = QString::number(value));
        tinicial(video,value,image);
        set_image_inicial(image);
        if(frame_fim<frame_ini){
            ui->FinalSlider->setValue(frame_ini+1);
            ui->T_final->setText(QString::number(frame_ini+1));
            frame_fim=frame_ini+1;
        }
    }
}



void zebtrack::on_FinalSlider_valueChanged(int value)
{
    if (ui->figura->pixmap()==0){

    }
    else if (frame_ini<value){
        ui->T_final->setText (s = QString::number(value));
        frame_fim=value;
        tfinal(video,value,image);
        set_image_final(image);
    }
    else{
        ui->FinalSlider->setValue(frame_ini+1);
        ui->T_final->setText(QString::number(frame_ini+1));
        frame_fim=frame_ini+1;
        tfinal(video,frame_fim,image);
        set_image_final(image);
    }
}

void zebtrack::on_frameBack_clicked()
{
    if(ui->figura->pixmap()==0){
        String="Video não selecionado";
        popup(String);
    }
    else if((video.get(CV_CAP_PROP_POS_FRAMES)-2)==0){
        String="Início do Vídeo";
        popup(String);
    }
    else{
    video.set(CV_CAP_PROP_POS_FRAMES,(video.get(CV_CAP_PROP_POS_FRAMES)-2));
    video>>image;
    setimage(image);
    }
}

void zebtrack::on_frameNext_clicked()
{
    if (ui->figura->pixmap()==0){
        String="Video não selecionado";
        popup(String);
    }
    else if(video.get(CV_CAP_PROP_POS_FRAMES) == video.get(CV_CAP_PROP_FRAME_COUNT)){
        String="Fim do Video";
        popup(String);
    }
    else{
    video>>image;
    setimage(image);
    }
}

void zebtrack::on_area_de_interesse_clicked()
{
    if(ui->figura->pixmap()!=0){
        area='i';
        emit desenhar();
    }
    else{
        String="Não há vídeo aberto";
        popup(String);
    }
}

void zebtrack::on_area_processamento_clicked()
{
    if(ui->figura->pixmap()!=0){
        area='p';
        emit desenhar();
    }
    else{
        String="Não há vídeo aberto";
        popup(String);
    }
}


void zebtrack::on_area_exclus_clicked()
{
    if(ui->figura->pixmap()!=0){
       area='e';
       emit desenhar();
    }
    else{
        String="Não há vídeo aberto";
        popup(String);
    }
}

void zebtrack::on_excluir_area_clicked()
{
   if(!excluiu){
        if (area=='i'){
            areas_inter.pop_back();
            excluiu=true;
        }
        else if(area=='p'){
            process.clear();
            excluiu=true;
        }
        else if(area=='e'){
            areas_exclus.pop_back();
            excluiu=true;
        }
        else{
            String="Nenhuma área definida";
            popup(String);
        }
        emit exclui();
   }
   else {
       String="Última área definida já foi excluida";
       popup(String);
   }
}

void zebtrack::on_checkBox_clicked()
{
    if(ui->figura->pixmap()==0){
        ui->checkBox_2->setChecked(false);
    }
    else{
        if(ui->checkBox->isChecked()==true){
            if(ui->checkBox_2->isChecked()==true){
                background=fundo_m_cor;
                setimage(var_cor);
            }
            else{
                background=fundo_m_cor;
                setimage(fundo_m_cor);
            }
            cor=true;
        }
        else{
            if(ui->checkBox_2->isChecked()==true){
                background=fundo_m_pb;
                setimage(var_pb);
            }
            else{
                background=fundo_m_pb;
                setimage(fundo_m_pb);
            }
            cor=false;
        }
    }
}

void zebtrack::on_recalc_fundo_clicked()
{
   if(video.isOpened()==true){
        calcfundo(video,image,skip);
        setimage(image);
   }
}

void zebtrack::on_SampleRate_valueChanged(int arg1)
{
    skip=arg1;
}

void zebtrack::on_checkBox_2_clicked() ///converter aqui pra mostrar em tela --> só pra testar
{
    if(ui->figura->pixmap()==0){
//        ui->checkBox_2->setChecked(false);
    }
    else{
        if(ui->checkBox_2->isChecked()==true){
            if(ui->checkBox->isChecked()==true){
//                var_cor.convertTo(fundo_v_cor,CV_8UC3);
                    setimage(var_cor);
                cor=true;
                background=fundo_m_cor;
            }
            else{
                background=fundo_m_pb;
//                fundo_v_pb.convertTo(fundo_v_pb,CV_8UC1);
                //quando mostrar elevar ao quadrado
                    setimage(var_pb);
                cor=false;
            }
            media=false;
        }
        else{
            if(ui->checkBox->isChecked()==true){
                background=fundo_m_cor;
                cor=true;
                setimage(background);
            }
            else{
                background=fundo_m_pb;
                setimage(background);
                cor=false;
            }
            media=true;
        }

    }
}

void zebtrack::on_threshold_box_valueChanged(int arg1)
{
    th=arg1;
}

void zebtrack::on_inicio_process_clicked()
{
    pause=false;
    ui->pausa_process->setText("Pausar");
    frameatual=0;
    passar_video(frameatual);
}
void zebtrack::passar_video(int &atual){
    if(ui->figura->pixmap()==0){
     String="Não há video aberto";
     popup(String);
    }
    else{
        QTextStream out(stdout);
        out << "inicio"<< endl;
        int i=atual;
        video.set(CV_CAP_PROP_POS_FRAMES,i);
        while(!pause&&i<=maxframe){
            Mat fore;
            Mat drawing = Mat::zeros( fore.size(), CV_8UC3 );
            video>>image;
            drawing = image.clone(); //testar assim
            drawing.convertTo(drawing,CV_8UC3); //testar assim
            fore=calculaForeground(image,background);
            processar.dilatErosion(fore);
            processar.find(fore,drawing);
            setimage(fore);
//            setimage(drawing);
            QCoreApplication::processEvents();
            i=i+rate;
//            out  << "i= "<< i <<endl;
            video.set(CV_CAP_PROP_POS_FRAMES,i);
        }
    }
}

void zebtrack::on_pausa_process_clicked()
{
    if(!pause){
        pause=!pause;
        frameatual=(video.get(CV_CAP_PROP_POS_FRAMES)-1);
        ui->pausa_process->setText("Continuar");
    }
    else{
        pause=!pause;
        ui->pausa_process->setText("Pausar");
        passar_video(frameatual);
    }

}

void zebtrack::on_back_process_clicked()
{
    if(ui->figura->pixmap()==0){
        String="Video não selecionado";
        popup(String);
    }
    else if((video.get(CV_CAP_PROP_POS_FRAMES)-(rate+1))==0){
        String="Início do Vídeo";
        popup(String);
    }
    else{
    video.set(CV_CAP_PROP_POS_FRAMES,(video.get(CV_CAP_PROP_POS_FRAMES)-(rate+1)));
    video>>image;
    Mat fore=calculaForeground(image,background);
    Mat drawing = Mat::zeros( fore.size(), CV_8UC3 );
    drawing = image.clone(); //testar assim
    drawing.convertTo(drawing,CV_8UC3); //testar assim
    setimage(drawing);
    processar.dilatErosion(fore);
    processar.find(fore,drawing);
//    setimage(fore);
    setimage(drawing); //testas assim
    }
}

void zebtrack::on_next_process_clicked()
{
    if (ui->figura->pixmap()==0){
        String="Video não selecionado";
        popup(String);
    }
    else if(video.get(CV_CAP_PROP_POS_FRAMES) == video.get(CV_CAP_PROP_FRAME_COUNT)){
        String="Fim do Video";
        popup(String);
    }
    else{
    video.set(CV_CAP_PROP_POS_FRAMES,(video.get(CV_CAP_PROP_POS_FRAMES)+rate));
    video>>image;
    Mat fore=calculaForeground(image,background);
    Mat drawing = Mat::zeros( fore.size(), CV_8UC3 );
    drawing = image.clone(); //testar assim
    drawing.convertTo(drawing,CV_8UC3); //testar assim
    setimage(drawing);
    processar.dilatErosion(fore);
    processar.find(fore,drawing);
//    setimage(fore);
    setimage(drawing);
    }
}

void zebtrack::on_rate_Box_valueChanged(int arg1)
{
    rate=arg1;
}
