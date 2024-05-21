#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDialog>
#include <QString>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <stack>
#include <QString>
#include <imgMananger.h>
#include <tesseract.h>
#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>
#include <QCamera>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QImageCapture>
#include "scanner.h"
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    vector<Point> initalPoints; // 初始点
    vector<Point> finalPoints; // 初始点经过固定排序之后的点
    float w = 420, h = 596;
    Mat imgCrops,imgWarp;
    QList<QCameraDevice> camerasList = QMediaDevices::videoInputs();
    QMediaCaptureSession captureSession;
    QScopedPointer<QCamera> m_camera;
    QCameraDevice tempDevice;
    QImageCapture* imageCapture;
    bool camerastatus;
    imgMananger<Mat> imgContainer;
    imgMananger<Mat> imgContainer_1;
    fontMananger<Mat> Tesseract;
    Scanner<Mat> scanner;
    void imgUpdate(QLabel*,Mat);//控件更新显示
    QImage cvMat2QImage(const Mat &mat);
    Mat cvQImage2Mat(const QImage &image);
    Mat pixmapToMat(const QPixmap &pixmap);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
    // tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
    bool imgOpened = false;
    Mat cvImg;
    Mat captureImg;

private slots:
    void openImg(QLabel*, bool stroge);//从磁盘中选择待处理的图像
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_action_Open_triggered();
    void on_pushButton_ocr_clicked();
    void on_pushButton_cut_clicked();
    void on_pushButton_choose_clicked();
    void on_pushButton_back_clicked();
    void on_pushButton_back1_clicked();
    void on_pushButton_grey_clicked();
    void on_pushButton_soft_clicked();
    void on_pushButton_hard_clicked();
    void on_pushButton_cut1_clicked();
    void on_pushButton_choose1_clicked();
    void on_pushButton_reload_clicked();
    void on_pushButton_video_clicked();
    // void on_comboBox_currentIndexChanged(int index);
    void on_comboBox_activated(int index);
    void on_pushButton_video_2_clicked();
    void do_image_capture(int id, const QImage &img);
    void on_pushButton_toocr_clicked();
    void on_action_Save_triggered();
    void SaveImg(Mat img);
    void on_actionaction_cut_triggered();
    void onCompleteCature(QPixmap captureImage);
    void onClose();
};
#endif // MAINWINDOW_H
