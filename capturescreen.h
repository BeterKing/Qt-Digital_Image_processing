#ifndef CAPTURESCREEN_H
#define CAPTURESCREEN_H

#include <QWidget>
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QWidget>
enum CaptureState {
    InitCapture,
    BeginMoveCaptureArea,
    FinishMoveCaptureArea,
    BeginCaptureImage,
    FinishCaptureImage,
    BeginMoveStretchRect,
    FinishMoveStretchRect,
    FinishCapture
};

enum StretchRectState {
    NotSelect,
    TopLeftRect,
    TopCenterRect,
    TopRightRect,
    RightCenterRect,
    BottomRightRect,
    BottomCenterRect,
    BottomLeftRect,
    LeftCenterRect
};

class CaptureScreen : public QWidget
{
    Q_OBJECT

public:
    CaptureScreen(QWidget *parent = 0);
    CaptureState m_currentCaptureState;
    StretchRectState m_stretchRectState;
    ~CaptureScreen();

Q_SIGNALS:
    void signalCompleteCature(QPixmap catureImage);
    void signalClose();

private:
    void initWindow();
    void loadBackgroundPixmap();
    bool isPressPointInSelectRect(QPoint mousePressPoint);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    void drawCaptureImage();
    void initStretchRect();
    void drawStretchRect();
    void setStretchCursorStyle(StretchRectState stretchRectState);
    QRect getSelectRect();
    QRect getRect(const QPoint &,const QPoint &);
    QRect getStretchRect();
    StretchRectState getStrethRectState(QPoint point);

private:
    qreal scaledRate;
    QPixmap m_loadPixmap, m_capturePixmap;
    int m_screenwidth;
    int m_screenheight;
    QPoint m_beginPoint, m_endPoint, m_beginMovePoint, m_endMovePoint;
    QPainter m_painter;
    QRect m_currentSelectRect;
    QRect getMoveRect();
    QPoint getMovePoint();
    QRect m_topLeftRect,m_topRightRect,m_bottomLeftRect,m_bottomRightRect
        ,m_leftCenterRect,m_topCenterRect,m_rightCenterRect,m_bottomCenterRect;
    QRect copyImg;
};


#endif // CAPTURESCREEN_H
