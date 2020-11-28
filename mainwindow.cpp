#include "mainwindow.h"
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

    connect(this, SIGNAL(windowStateChanged(Qt::WindowState)), this, SLOT(windowStateChange(Qt::WindowState)));//没有这个信号

    QString userName = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0).section('/', -1);
    // 登录设置文件：/var/lib/AccountsService/users/用户名
    QSettings *settings = new QSettings("/var/lib/AccountsService/deepin/users/" + userName, QSettings::IniFormat);
    QString DesktopBackgrounds = settings->value("/User/DesktopBackgrounds").toString();
    QStringList SL = DesktopBackgrounds.split(";");
    QString DesktopBackground = SL.at(0);
    DesktopBackground = DesktopBackground.replace("file://", "");
    QFile file(DesktopBackground);
    QString sstyle = "MainWindow { border-image:url(" + DesktopBackground + "); background:#000000;}"
            "QToolButton { color:white; font-size:16px; background:transparent; }"            
            "QToolButton::focus { background:rgba(0,0,0,100); }"
            "QToolButton::hover { color:black; background:rgba(255,255,255,100); }"
            "QToolButton::pressed { color:white; }";
    setStyleSheet(sstyle);
    showFullScreen();
    connect(new QShortcut(QKeySequence(Qt::Key_Escape),this), SIGNAL(activated()), qApp, SLOT(quit()));

    br = 0;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]{
        if (br < 20)
            br++;
        else
            timer->stop();
        QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
        blurEffect->setBlurRadius(br);
        setGraphicsEffect(blurEffect);
    });
    //timer->start(100);//全模糊
}

MainWindow::~MainWindow()
{

}

void MainWindow::windowStateChange(Qt::WindowState state)
{
    qDebug() << state;
    if (state == Qt::WindowActive) {
        br = 0;
        timer->start(100);
    }
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