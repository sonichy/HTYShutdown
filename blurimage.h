#ifndef BLURIMAGE_H
#define BLURIMAGE_H

#include <QObject>
#include <QImage>

class BlurImage : public QObject
{
    Q_OBJECT

public:
    explicit BlurImage(QImage image, int p, QObject *parent = nullptr);
    //~BlurImage();
    QImage image0, image_blur;
    int p0;

private:
    void blur(QImage image, int p);

signals:
    void finish();

public slots:
    void start();
};

#endif // BLURIMAGE_H