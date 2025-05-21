
#include "tetrixboard.h"
#include "tetrixwindow.h"

#include <QCoreApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLCDNumber>
#include <QPushButton>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QApplication>

TetrixWindow::TetrixWindow(QWidget *parent)
    : QWidget(parent), board(new TetrixBoard)
{
    QDesktopWidget dw;
    QWidget window;
    qInfo() << "Original Screen Size : Width: " << dw.width() << " Height: " << dw.height();
    window.resize(dw.width(), 1024);

    qApp->installEventFilter(this);
    QApplication::setOverrideCursor(Qt::BlankCursor);

    QString lcd_style = "color:white; border: 1px solid cyan;";

    nextPieceLabel = new QLabel;
    nextPieceLabel->setFrameStyle(QFrame::Box | QFrame::Raised);
    nextPieceLabel->setAlignment(Qt::AlignCenter);
    nextPieceLabel->setStyleSheet("border: 1px solid cyan;");
    board->setNextPieceLabel(nextPieceLabel);

    scoreLcd = new QLCDNumber(5);
    scoreLcd->setSegmentStyle(QLCDNumber::Filled);
    scoreLcd->setStyleSheet(lcd_style);

    levelLcd = new QLCDNumber(5);
    levelLcd->setSegmentStyle(QLCDNumber::Filled);
    levelLcd->setStyleSheet(lcd_style);

    linesLcd = new QLCDNumber(5);
    linesLcd->setSegmentStyle(QLCDNumber::Filled);
    linesLcd->setStyleSheet(lcd_style);

    portTimer = new QTimer(this);
    portTimer->setInterval(200);
    connect(portTimer,SIGNAL(timeout()),this,SLOT(refreshGPIO()));
    portTimer->start();

#if __cplusplus >= 201402L
    connect(board, &TetrixBoard::scoreChanged,
            scoreLcd, qOverload<int>(&QLCDNumber::display));
    connect(board, &TetrixBoard::levelChanged,
            levelLcd, qOverload<int>(&QLCDNumber::display));
    connect(board, &TetrixBoard::linesRemovedChanged,
            linesLcd, qOverload<int>(&QLCDNumber::display));
#else
    connect(board, &TetrixBoard::scoreChanged,
            scoreLcd, QOverload<int>::of(&QLCDNumber::display));
    connect(board, &TetrixBoard::levelChanged,
            levelLcd, QOverload<int>::of(&QLCDNumber::display));
    connect(board, &TetrixBoard::linesRemovedChanged,
            linesLcd, QOverload<int>::of(&QLCDNumber::display));
#endif

    QGridLayout *layout = new QGridLayout;

    QLabel *lblCaption;
    lblCaption = new QLabel(this);
    lblCaption->setText("°º°°º° TETRIS °º°°º°");
    lblCaption->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lblCaption->setStyleSheet("/*background-color:yellow;*/ color:white; font:60px; border: 5px solid cyan;");

    layout->addWidget(lblCaption, 0, 0, 1, 6);
    layout->addWidget(createLabel(tr("Sonraki / Next")), 1, 0, 1, 2);
    layout->addWidget(nextPieceLabel, 2, 0, 2, 2);

    layout->addWidget(createLabel(tr("Seviye / Level")), 4, 0, 1, 2);
    layout->addWidget(levelLcd, 5, 0, 1, 2);

    layout->addWidget(createLabel(tr("Puan / Score")), 6, 0, 1, 2);
    layout->addWidget(scoreLcd, 7, 0, 1, 2);

    layout->addWidget(createLabel(tr("Silinen / Removed")), 8, 0, 1, 2);
    layout->addWidget(linesLcd, 9, 0, 1, 2);

    layout->addWidget(createLabel(tr("Oyun Alan / Game Board")), 1, 2, 1, 4);
    layout->addWidget(board, 2, 2, 8, 4);

    board->setStyleSheet("border: 1px solid cyan;");

    setLayout(layout);
    setFont(QFont("Liberation Sans"));
    setStyleSheet("background-color:black;");
    setWindowTitle(tr("Butkon-TetriX"));
    resize(600, 1024);
}

QLabel *TetrixWindow::createLabel(const QString &text)
{
    QLabel *label = new QLabel(text);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    label->setStyleSheet("color:white;");
    return label;
}

int TetrixWindow::getGpioValue(unsigned int gpio)
{
    int fd,value;
    char temp;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/value", gpio);
    fd = open(buf, O_RDONLY);
    if(fd < 0)
    {
        printf("gpio%d get value failed - FD:%d!\n", gpio, fd);
        return -1;
    }

    read(fd, &temp, 1);
    if(temp != '0')
    {
        value = 1;
    }
    else
    {
        value = 0;
    }
    ::close(fd);
    return value;
}

void TetrixWindow::refreshGPIO()
{
    for (int i = 132; i < 139; i++) {
        int state = getGpioValue(i);

        if (state == 1){
            qDebug() << "PORT_NUM : " << i << " State: " << state;
            switch (i) {
            case 132:
                qInfo() << "KEY_START:: run run run";
                board->start();
                break;

            case 133:
                qInfo() << "KEY_DOWN :: vvv vvv vvv";
                //oneLineDown();
                board->sendKBKey(Qt::Key_D);
                break;

            case 134:
                qInfo() << "KEY_LEFT :: <<< <<< <<<";
                board->sendKBKey(Qt::Key_Left);
                break;

            case 135:
                qInfo() << "KEY_RIGHT :: >>> >>> >>>";
                board->sendKBKey(Qt::Key_Right);
                break;

            case 136:
                qInfo() << "KEY_ROTATE :: ooo ooo ooo";
                board->sendKBKey(Qt::Key_Down);
                break;

            case 137:
                break;
            case 138:
                break;
            default:
                break;
            }
        }
    }
}

bool TetrixWindow::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::MouseMove)
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    qInfo() << (QString("Moved! (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));
    QApplication::restoreOverrideCursor();
  }
  return false;
}
