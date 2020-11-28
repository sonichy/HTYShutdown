#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private:
    QToolButton *toolButton_shutdown, *toolButton_reboot, *toolButton_suspend, *toolButton_lock, *toolButton_logout;
    QTimer *timer;
    int br;

private slots:
    void windowStateChange(Qt::WindowState state);

};

#endif // MAINWINDOW_H