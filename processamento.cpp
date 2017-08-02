#include "processamento.h"
#include <QTextStream>

using namespace cv;
using namespace std;

Process::Process()
{


}

void Process::dilatErosion(Mat &fore){
//   Mat Kernel= getStructuringElement(cv::MORPH_RECT,cv::Size(5),cv::Point(-1,-1));
   int morph_size=3;
   Mat element = getStructuringElement( MORPH_RECT, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
   erode(fore,fore,cv::Mat());
//   dilate(fore,fore,element);
   dilate(fore,fore,cv::Mat());
}

void Process::find(Mat fore,Mat drawing){
    Mat contorno;
    QTextStream out(stdout);
    vector<vector<cv::Point> > contours;
    vector<Point2f> cm;
    vector<Vec4i> hierarchy;
    fore.convertTo(contorno,CV_8UC1);

    findContours( contorno, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
//    nfore=Scalar::all(0);
//    nfore.convertTo(nfore,CV_32FC1); //testando
//    drawContours(nfore,contours,-1,Scalar::all(255)); //testar desenhar os contornos numa nova imagem


//    out << "contours" <<contours.size() << endl;

    if(contours.size() > 0)
    {
        /// Approximate contours to polygons + get bounding rects and circles
        vector<vector<Point> > contours_poly( contours.size() );
        vector<Rect> boundRect(contours.size());
//        boundRect.resize(contours.size()); se declarado como parametro da função usar esse aqui
        vector<Point2f>center( contours.size() );
        vector<float>radius( contours.size() );
        vector<Moments> mu(contours.size());

//        cout << "::: Area total da imagem = " << imgSeguimentada.cols * imgSeguimentada.rows << " ::: " << endl << endl;

        for( int i = 0; i < contours.size(); i++)
        {

            approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
            boundRect.push_back(boundingRect( Mat(contours_poly[i]) )); ///TESTAR SE O BOUND TA SALVANDO MESMO


            minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
            mu[i] = moments( contours[i], false);

//            out << "--- Area do blob: " << mu[i].m00 << endl;

            if(mu[i].m00 > 5 && mu[i].m00 <= (fore.cols*fore.rows)/2 ){
//                nbarcosAchados++;
//                fore.convertTo(fore,CV_8UC3);
                Scalar color2 = Scalar( 0,255,0  );
                Scalar color = Scalar( 255,255,255  ); //Cor do retângulo Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) )//
//                circle(fore,Point(100,100),70,Scalar(0,0,0),5,8,0);
                rectangle( fore, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 ); // Desenha retângulo nos blobs
                rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color2, 2, 8, 0 );
//                vector<Point2f> cm;
//                fore.convertTo(fore,CV_8UC1);
//                namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
//                imshow( "Display window", drawing );
//                system("pause");
                cm.push_back( Point2f(mu[i].m10/mu[i].m00, mu[i].m01/mu[i].m00)); // Calculando centro de massa


//                out << "Centro de massa: " << cm[i].x << " " << cm[i].y << endl;
//                out << endl << endl;

//                int cantosuperiorleftx=boundRect[i].tl().x;
//                int cantosuperiorlefty=boundRect[i].tl().y;
//                int cantoinferiorrightx=boundRect[i].br().x;
//                int cantoinferiorrighty=boundRect[i].br().y;


//                centrox = boundRect[i].tl().x + tamx/2;//Centro dos retângulos
//                centroy = boundRect[i].tl().y + tamy/2;

//                Mat imgCortada(imgORG, Rect(boundRect[i].tl().x, boundRect[i].tl().y, tamx, tamy) );
                //mostraimagem("Cortada", imgCortada);
                //sprintf(nome, "0%d.jpeg", i + 27);
                //imwrite(nome, imgCortada);

//                line(imgORG, {(centrox - 3), (centroy - 3)} , {(centrox + 3), (centroy + 3)}, color, 2, CV_AA, 0); //Desenha X nos centros
//                line(imgORG, {(centrox - 3), (centroy + 3)} , {(centrox + 3), (centroy - 3)}, color, 2, CV_AA, 0); //dos retângulos
            }
        }
    }
}
