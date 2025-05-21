#ifndef TETRIXWINDOW_H
#define TETRIXWINDOW_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <QWidget>
#include <QTimer>
#include <QDebug>

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 256

QT_BEGIN_NAMESPACE
class QLCDNumber;
class QLabel;
class QPushButton;
QT_END_NAMESPACE
class TetrixBoard;

class TetrixWindow : public QWidget
{
    Q_OBJECT

public:
    TetrixWindow(QWidget *parent = nullptr);
    int getGpioValue(unsigned int gpio);
public slots:
    void refreshGPIO();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QLabel *createLabel(const QString &text);

    TetrixBoard *board;
    QLabel *nextPieceLabel;
    QLCDNumber *scoreLcd;
    QLCDNumber *levelLcd;
    QLCDNumber *linesLcd;
    QTimer *portTimer;
};

#endif
