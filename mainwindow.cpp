#include "mainwindow.h"
#include "blurimage.h"
#include <QStandardPaths>
#include <QSettings>
#include <QFile>
#include <QGraphicsBlurEffect>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QShortcut>
#include <QApplication>
#include <QPainter>
#include <QDesktopWidget>
#include <QThread>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{    
    setWindowIcon(QIcon(":/icon/shutdown.svg"));

    QWidget *widget = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addStretch();
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setSpacing(50);
    hbox->addStretch();

    toolButton_shutdown = new QToolButton;
    toolButton_shutdown->setIcon(QIcon(":/icon/shutdown.svg"));
    toolButton_shutdown->setIconSize(QSize(50, 50));
    toolButton_shutdown->setText("关机");
    toolButton_shutdown->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolButton_shutdown->setFixedSize(100, 120);
    toolButton_shutdown->installEventFilter(this);
    connect(toolButton_shutdown, &QToolButton::pressed, []{
        //QString command = "dbus-send --print-reply --dest=com.deepin.SessionManager /com/deepin/SessionManager  com.deepin.SessionManager.RequestShutdown";
        QString command = "init 0";
        QProcess::startDetached(command);
    });
    hbox->addWidget(toolButton_shutdown);

    toolButton_reboot = new QToolButton;
    toolButton_reboot->setIcon(QIcon(":/icon/reboot.svg"));
    toolButton_reboot->setIconSize(QSize(50, 50));
    toolButton_reboot->setText("重启");
    toolButton_reboot->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolButton_reboot->setFixedSize(100, 120);
    toolButton_reboot->installEventFilter(this);
    connect(toolButton_reboot, &QToolButton::pressed, []{
        //QString command = "dbus-send --print-reply --dest=com.deepin.SessionManager /com/deepin/SessionManager  com.deepin.SessionManager.RequestReboot";
        QString command = "init 6";
        QProcess::startDetached(command);
    });
    hbox->addWidget(toolButton_reboot);

    toolButton_suspend = new QToolButton;
    toolButton_suspend->setIcon(QIcon(":/icon/suspend.svg"));
    toolButton_suspend->setIconSize(QSize(50, 50));
    toolButton_suspend->setText("待机");
    toolButton_suspend->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolButton_suspend->setFixedSize(100, 120);
    toolButton_suspend->installEventFilter(this);
    connect(toolButton_suspend, &QToolButton::pressed, []{
        QString command = "dbus-send --print-reply --dest=com.deepin.SessionManager /com/deepin/SessionManager  com.deepin.SessionManager.RequestSuspend";
        QProcess::startDetached(command);
    });
    hbox->addWidget(toolButton_suspend);

    toolButton_lock = new QToolButton;
    toolButton_lock->setIcon(QIcon(":/icon/lock.svg"));
    toolButton_lock->setIconSize(QSize(50, 50));
    toolButton_lock->setText("锁定");
    toolButton_lock->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolButton_lock->setFixedSize(100, 120);
    toolButton_lock->installEventFilter(this);
    connect(toolButton_lock, &QToolButton::pressed, []{
        QString command = "dbus-send --print-reply --dest=com.deepin.SessionManager /com/deepin/SessionManager  com.deepin.SessionManager.RequestLock";
        QProcess::startDetached(command);
    });
    hbox->addWidget(toolButton_lock);

    toolButton_logout = new QToolButton;
    toolButton_logout->setIcon(QIcon(":/icon/logout.svg"));
    toolButton_logout->setIconSize(QSize(50, 50));
    toolButton_logout->setText("注销");
    toolButton_logout->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolButton_logout->setFixedSize(100, 120);
    toolButton_logout->installEventFilter(this);
    connect(toolButton_logout, &QToolButton::pressed, []{
        QString command = "dbus-send --print-reply --dest=com.deepin.SessionManager /com/deepin/SessionManager  com.deepin.SessionManager.RequestLogout";
        QProcess::startDetached(command);
    });
    hbox->addWidget(toolButton_logout);

    hbox->addStretch();
    vbox->addLayout(hbox);
    vbox->addStretch();
    widget->setLayout(vbox);
    setCentralWidget(widget);

    QString user_name = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).section('/', -1);
    // 登录设置文件：/var/lib/AccountsService/users/用户名
    //QSettings *settings = new QSettings("/var/lib/AccountsService/deepin/users/" + user_name, QSettings::IniFormat);
    //QString DesktopBackgrounds = settings->value("/User/DesktopBackgrounds").toString();//QSettings 逗号分隔，分号结束，只能读到第一张壁纸
    QString filepath = "/var/lib/AccountsService/deepin/users/" + user_name;
    QFile file(filepath);
    file.open(QIODevice::ReadOnly);
    QString s = file.readAll();
    QStringList SL = s.split("\n");
    QString DesktopBackgrounds = "";
    for (int i=0; i<SL.length(); i++) {
        s = SL.at(i);
        if (s.startsWith("DesktopBackgrounds=")) {
            DesktopBackgrounds = s.replace("DesktopBackgrounds=", "");
        }
    }
    qWarning() << DesktopBackgrounds;
    SL = DesktopBackgrounds.split(";");
    SL.removeAll("");
    qWarning() << SL;
    QString DesktopBackground = SL.at(0);
    DesktopBackground = DesktopBackground.replace("file://", "");
    qWarning() << DesktopBackground;
    image_bg = QImage(DesktopBackground);
    QString sstyle =
            //"MainWindow { border-image:url(" + DesktopBackground + "); background:#000000;}"
            "QToolButton { color:white; font-size:16px; background:transparent; }"            
            "QToolButton::focus { background:rgba(0,0,0,100); }"
            "QToolButton::hover { color:black; background:rgba(255,255,255,100); }"
            "QToolButton::pressed { color:white; }";
    setStyleSheet(sstyle);
    showFullScreen();    

    QTime startTime = QTime::currentTime();
    QThread *thread = new QThread(this);
    QImage image(DesktopBackground);
    BlurImage *blurImage = new BlurImage(image, 5);
    connect(blurImage, &BlurImage::finish, [=]{
        qWarning() << "draw blur background";
        image_bg = blurImage->image_blur;
        update();
        thread->quit();
        QTime stopTime = QTime::currentTime();
        int elapsed = startTime.msecsTo(stopTime);
        QTime time(0,0,0);
        time = time.addMSecs(elapsed);
        qWarning() << "模糊背景耗时：" << time.toString("mm:ss.zzz");
    });
    blurImage->moveToThread(thread);
    thread->start();
    connect(this, &MainWindow::startThread, blurImage, &BlurImage::start);
    emit startThread();

    connect(new QShortcut(QKeySequence(Qt::Key_Escape),this), &QShortcut::activated, [=]{
        thread->quit();
        //thread->wait();//进程没结束退出会崩溃，等待等下才能退出。
        qApp->quit();
    });

    //blur(image_bg, 5);//卡UI
}

MainWindow::~MainWindow()
{

}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == toolButton_shutdown) {        
        if (event->type() == QEvent::Enter || event->type() == QEvent::MouseButtonRelease) {
            toolButton_shutdown->setIcon(QIcon(":/icon/shutdown_hover.svg"));
        } else if (event->type() == QEvent::Leave || event->type() == QEvent::MouseButtonPress) {
            toolButton_shutdown->setIcon(QIcon(":/icon/shutdown.svg"));
        }
    } else if (object == toolButton_reboot) {
        if (event->type() == QEvent::Enter || event->type() == QEvent::MouseButtonRelease) {
            toolButton_reboot->setIcon(QIcon(":/icon/reboot_hover.svg"));
        } else if (event->type() == QEvent::Leave || event->type() == QEvent::MouseButtonPress) {
            toolButton_reboot->setIcon(QIcon(":/icon/reboot.svg"));
        }
    } else if (object == toolButton_suspend) {
        if (event->type() == QEvent::Enter || event->type() == QEvent::MouseButtonRelease) {
            toolButton_suspend->setIcon(QIcon(":/icon/suspend_hover.svg"));
        } else if (event->type() == QEvent::Leave || event->type() == QEvent::MouseButtonPress) {
            toolButton_suspend->setIcon(QIcon(":/icon/suspend.svg"));
        }
    } else if (object == toolButton_lock) {
        if (event->type() == QEvent::Enter || event->type() == QEvent::MouseButtonRelease) {
            toolButton_lock->setIcon(QIcon(":/icon/lock_hover.svg"));
        } else if (event->type() == QEvent::Leave || event->type() == QEvent::MouseButtonPress) {
            toolButton_lock->setIcon(QIcon(":/icon/lock.svg"));
        }
    } else if (object == toolButton_logout) {
        if (event->type() == QEvent::Enter || event->type() == QEvent::MouseButtonRelease) {
            toolButton_logout->setIcon(QIcon(":/icon/logout_hover.svg"));
        } else if (event->type() == QEvent::Leave || event->type() == QEvent::MouseButtonPress) {
            toolButton_logout->setIcon(QIcon(":/icon/logout.svg"));
        }
    }
    return QWidget::eventFilter(object, event);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //qWarning() << "draw background";
    Q_UNUSED(event);    
    QPainter painter(this);
    painter.drawImage(rect(), image_bg);
}

void MainWindow::blur(QImage image, int p)
{
    image_bg = QImage(image.width(), image.height(), QImage::Format_RGB32);
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
            image_bg.setPixel(x, y, RGBblur);
        }
    }
    qWarning() << "QImage blur finish";
    update();
}