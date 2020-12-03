#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "blurimage.h"
#include <QMainWindow>
#include <QToolButton>
#include <QTimer>
#include <QEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QToolButton *toolButton_shutdown, *toolButton_reboot, *toolButton_suspend, *toolButton_lock, *toolButton_logout;
    QImage image_bg;    
    void blurBackground(QImage image);
    void blur(QImage image, int p);

signals:
    void startThread();

};

#endif // MAINWINDOW_H