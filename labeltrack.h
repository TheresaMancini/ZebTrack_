#ifndef LABELTRACK_H
#define LABELTRACK_H
#include <QLabel>
#include <QPoint>
#include <QPolygon>
#include <zebtrack.h>


class labeltrack: public QLabel{
    Q_OBJECT
 public:
    labeltrack( QWidget* parent=0);
    //other methods
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent ( QMouseEvent * event );
    void paintEvent(QPaintEvent* ev);
    void resizeEvent ( QResizeEvent * event );
    void mouseDoubleClickEvent ( QMouseEvent * event );
    short int nc;
    QPolygon poly;
//    QVector <QPoint> vpontos;
    bool final=false,mov=false;

private:
    QPoint click1,click2;
    bool desenhar=false; //incluiu desenhar

private slots:
    void desenhando(); // incluiu desenhando
    void area_excluiuda();
signals:

    void mudou_tamanho();
    void desenhou();

};

#endif // LABELTRACK_H
