//QThread：https://blog.csdn.net/qq_27085429/article/details/86480953
#include "blurimage.h"
#include <QDebug>

BlurImage::BlurImage(QImage image, int p, QObject *parent)
{
    Q_UNUSED(parent);
    image0 = image;
    p0 = p;
}

void BlurImage::start()
{
    blur(image0, p0);
}

void BlurImage::blur(QImage image, int p)
{
    image_blur = QImage(image.width(), image.height(), QImage::Format_RGB32);
    for(int x=0; x<image.width(); x++){
        for(int y=0; y<image.height(); y++){
            int red=0, green=0, blue=0, pc=0;
            for(int a=-p; a<=p; a++){
                for(int b=-p; b<=p; b++){
                    int xa = x+a;
                    int yb = y+b;
                    if(xa>0 && yb>0 && xa<image.width() && yb<image.height()){
                        red += qRed(image.pixel(xa,yb));
                        green += qGreen(image.pixel(xa,yb));
                        blue += qBlue(image.pixel(xa,yb));
                        pc += 1;
                    }
                }
            }
            QRgb RGBblur = qRgb(red/pc, green/pc, blue/pc);
            image_blur.setPixel(x, y, RGBblur);
        }
    }
    qWarning() << "QImage blur finish";
    emit finish();
}