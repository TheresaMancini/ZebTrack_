#include "labeltrack.h"
#include <QTextStream>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>

labeltrack::labeltrack( QWidget* parent)
    : QLabel(parent)
{
    nc=0;
    this->setMouseTracking(true);
    this->click1.setX(0);
    this->click1.setY(0);
    this->click2.setX(0);
    this->click2.setY(0);
}

void labeltrack::mouseMoveEvent(QMouseEvent *event)
{
   //mouseX = event.x();
   //mouseY = event.y();
//    QTextStream out(stdout);
//   out  << "x: "<< event->pos().x() <<endl;
//   out << "y: "<<event->pos().y() <<endl;
   this->click2.setX(event->pos().x());
   this->click2.setY(event->pos().y());

   if(final==false && desenhar==true){
       if(mov==false){
       poly<<click2;
       mov=true;
       }
       else{
           poly.pop_back();
           poly<<click2;
       }
   }
   this->update();

}

void labeltrack::mousePressEvent ( QMouseEvent * event )
{
    final=false;
     QTextStream out(stdout);
     nc++;
     this->click1=event->pos();
     if (desenhar==true){
     poly<<click1;
     }
//     out  << "clicou uma vez nc="<<nc <<endl;
     mov=false;
}

void labeltrack::mouseDoubleClickEvent ( QMouseEvent * event )
    {
    final=true;
    update();
    emit desenhou(); // incluiu agora para teste
    desenhar=false;
}

void labeltrack::paintEvent(QPaintEvent* ev) {
      QLabel::paintEvent(ev); // do the default label display
      QPainter painter(this);
      QPen paintpen(Qt::black);
      QPen interpen(Qt::red);
      QPen excluspen(Qt::blue);
      QPen processpen(Qt::green);

      char tipo=((zebtrack*)(parent()->parent()))->area;
      if(tipo=='i'){
          interpen.setWidth(2);
          painter.setPen(interpen);
      }
      else if(tipo=='p'){
          processpen.setWidth(2);
          painter.setPen(processpen);
      }
      else if(tipo=='e'){
          excluspen.setWidth(2);
          painter.setPen(excluspen);
      }
      else
          painter.setPen(paintpen);

       if(desenhar==true){
          if(nc<2){
              painter.drawLine(poly.point(0),this->click2);
          }
          else /*if(nc>=2)*/{
              painter.drawPolygon(poly);
//              QTextStream out(stdout);
//              out  <<"AQIUWIQUWI=" << ((zebtrack*)(parent()->parent()))->x << endl;
          }

      }
      if(((zebtrack*)(parent()->parent()))->areas_exclus.size()!=0||((zebtrack*)(parent()->parent()))->areas_inter.size()!=0
              ||((zebtrack*)(parent()->parent()))->process.empty()==false){
          for (int i=0;i<((zebtrack*)(parent()->parent()))->areas_inter.size();i++){
              painter.setPen(interpen);
              painter.drawPolygon(((zebtrack*)(parent()->parent()))->areas_inter[i]);
          }
          for (int i=0;i<((zebtrack*)(parent()->parent()))->areas_exclus.size();i++){
              painter.setPen(excluspen);
              painter.drawPolygon(((zebtrack*)(parent()->parent()))->areas_exclus[i]);
          }
          painter.setPen(processpen);
          painter.drawPolygon(((zebtrack*)(parent()->parent()))->process);
      }

}
void labeltrack::desenhando(){
    desenhar=true;
}

void labeltrack::area_excluiuda(){
    update();
}

void labeltrack::resizeEvent ( QResizeEvent * event ){
     emit mudou_tamanho();
}
