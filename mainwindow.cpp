#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imgcrop.h"
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("简易数字图像处理");
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->comboBox->clear(); //清除列表
    if (!camerasList.isEmpty()) {
        foreach (const QCameraDevice &camera, camerasList) {
            QIcon icon;
            icon.addFile(":/image/icons/camera.png");
            ui->comboBox->addItem(icon, camera.description()); //带图标
        }
    } else {
        qDebug() << "No cameras found";
    }
    tempDevice = camerasList.first();
    camerastatus = false;
    imageCapture = new QImageCapture(this);
    captureSession.setImageCapture(imageCapture);
    connect(imageCapture,&QImageCapture::imageCaptured,this,&MainWindow::do_image_capture);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked() //文字识别
{
    if(ui->stackedWidget_2->currentIndex()!=0)
    {
        ui->stackedWidget->setCurrentIndex(0);
        ui->stackedWidget_2->setCurrentIndex(0);
        // if(!imgContainer.isEmpty()) imgContainer.cleanDealed();
    }
}


void MainWindow::on_pushButton_2_clicked() //图像处理
{
    if(ui->stackedWidget_2->currentIndex()!=1)
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->stackedWidget_2->setCurrentIndex(1);
        // if(!imgContainer.isEmpty()) imgContainer.cleanDealed();
    }
}


void MainWindow::on_pushButton_3_clicked() //文档扫描
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->stackedWidget_2->setCurrentIndex(2);
}

void MainWindow::on_pushButton_reload_clicked() //重新加载摄像头列表
{
    camerasList = QMediaDevices::videoInputs();
    ui->comboBox->clear(); //清除列表
    if (!camerasList.isEmpty()) {
        foreach (const QCameraDevice &camera, camerasList) {
            QIcon icon;
            icon.addFile(":/image/icons/camera.png");
            ui->comboBox->addItem(icon, camera.description()); //带图标
        }
    } else {
        qDebug() << "No cameras found";
    }
}

void MainWindow::on_pushButton_video_clicked()  //打开摄像头
{
    m_camera.reset(new QCamera(tempDevice));


    captureSession.setCamera(m_camera.data());
    captureSession.setVideoOutput(ui->widget);

    camerastatus = true;

    m_camera->start();

}

void MainWindow::on_comboBox_activated(int index)  //选择摄像头
{
    tempDevice = camerasList.at(index);
    m_camera.reset(new QCamera(tempDevice));

    captureSession.setCamera(m_camera.data());
    captureSession.setVideoOutput(ui->widget);

    if(camerastatus) m_camera->start();
}

void MainWindow::on_pushButton_video_2_clicked()  //拍照
{
    imageCapture->capture();
}

void MainWindow::do_image_capture(int id, const QImage &img)  //触发截图事件
{
    captureImg = cvQImage2Mat(img);
    ui->label_5->setPixmap(QPixmap::fromImage(img).scaled(ui->label_5->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void MainWindow::on_pushButton_toocr_clicked()  //文档扫描
{
    Mat imgThre = scanner.preProcessing(captureImg);
    initalPoints = scanner.getContours(imgThre,captureImg);

    if (initalPoints.size() != 0)
    {
        finalPoints = scanner.reorders(initalPoints);
        if (finalPoints.size() != 0)
        {
            imgWarp = scanner.getWarp(captureImg, finalPoints, w, h);
            int cropNum = 5;
            Rect roi(cropNum, cropNum, w - 2 * cropNum, h - 2 * cropNum);
            imgCrops = imgWarp(roi);
        }
    }
    else  // 若没有文档在视角范围内，则放置一个白板。
    {
        imgWarp = Mat(w, h, CV_8UC3, Scalar(255, 255, 255));
        imgCrops = Mat(w, h, CV_8UC3, Scalar(255, 255, 255));
    }
    captureImg = imgCrops;
    ui->label_5->setPixmap(QPixmap::fromImage(cvMat2QImage(imgCrops).scaled(ui->label_5->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation)));
}

void MainWindow::on_pushButton_choose_clicked() //选择图像
{
    openImg(ui->label,true);
}

void MainWindow::on_pushButton_choose1_clicked()  //选择图像
{
    openImg(ui->label_2,false);
}

void MainWindow::on_pushButton_ocr_clicked()  //文字识别
{
    QString outText;

    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();

    ocr->Init("E:/qtcreator/opencv_test/opencv_test/tessdata", "chi_sim", tesseract::OEM_DEFAULT);

    ocr->SetPageSegMode(tesseract::PSM_AUTO_OSD);

    Mat gray = Tesseract.grayImages(cvImg);

    // cv::imshow("imga", gray);

    Mat dilation = Tesseract.preprocess(gray);

    // cv::imshow("img", dilation);

    vector<RotatedRect> rects = Tesseract.findTextRegion(dilation);

    qDebug() << rects.size();

    for(int i = rects.size()-1; i>=0 ; i--)
    {
        vector<Point2f> P;
        rects[i].points(P);

        vector<Point2f> docPioints = Tesseract.reorder(P);
        Mat imgCrop = Tesseract.Croping(docPioints,cvImg);

        cv::imshow("123",imgCrop);
        ocr->SetImage(imgCrop.data, imgCrop.cols, imgCrop.rows, 3, imgCrop.step);

        ocr->Recognize(0);

        outText =outText + "\n" + QString::fromStdString(Tesseract.removeExtraSpacesAndNewlines(string(ocr->GetUTF8Text())));
    }

    ui->plainTextEdit->setPlainText(outText);
    qDebug() << outText;
}

void MainWindow::on_action_Open_triggered() //打开文件
{
    int index = ui->stackedWidget_2->currentIndex();
    switch (index) {
    case 0:
        openImg(ui->label, true);
        break;
    case 1:
        openImg(ui->label_2, false);
        break;
    }
}

void MainWindow::on_action_Save_triggered() //保存文件
{
    int index = ui->stackedWidget_2->currentIndex();
    QPixmap pixmap;
    QImage img;
    switch (index) {
    case 1:
        pixmap = ui->label_3->pixmap();
        img = pixmap.toImage();
        SaveImg(cvQImage2Mat(img));
        break;
    case 2:
        SaveImg(captureImg);
        break;
    }
}

void MainWindow::SaveImg(Mat img)
{
    QString curDir=QDir::currentPath();//选择文件
    QString filename=QFileDialog::getSaveFileName(this,"save Image",curDir,
                                                    "Images (*.png *.bmp *.jpg *.tif *.GIF )");
    if(filename.isEmpty()||img.empty())
    {
            qDebug() << "Save";
        return;
    }
    std::string save_path = filename.toLocal8Bit().toStdString();

    //save_path+=".jpg";//std::vector<int> compression_params;
    //compression_params.push_back(IMWRITE_JPEG_QUALITY);
    //compression_params.push_back(100);
    imwrite(save_path,img);//保存图片
}


void MainWindow::on_pushButton_cut_clicked()
{
    QPixmap pixmap = ui->label->pixmap();

    if(!pixmap.isNull())
    {
        imgCrop* Crop = new imgCrop(pixmap,this);

        int ret=Crop->exec();
        if(ret==QDialog::Accepted){
            ui->label->setPixmap(Crop->resultImage.scaled(ui->label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
            cvImg = cvQImage2Mat(Crop->resultImage.toImage());
            imgContainer.push(cvImg);
        }
        delete Crop;
    }else {
        QMessageBox::critical(this,"错误信息","请先选择图片");
    }
}

void MainWindow::on_pushButton_back_clicked()
{
    if(!imgContainer.isEmpty())
    {
        Mat backImg = imgContainer.back();
        imgUpdate(ui->label, backImg);
    }
}

void MainWindow::on_pushButton_back1_clicked()
{
    if(!imgContainer_1.isEmpty())
    {
        Mat backImg = imgContainer_1.back();
        imgContainer_1.setTemp(backImg);
        imgUpdate(ui->label_3, backImg);
    }
}

void MainWindow::on_pushButton_grey_clicked()
{
    Mat img = imgContainer_1.getTemp();
    if(img.type()!=CV_8UC1)
    {
        Mat grey = Tesseract.grayImages(img);

        imgUpdate(ui->label_3,grey);
        imgContainer_1.push(grey);
    }
}

void MainWindow::on_pushButton_soft_clicked()
{
    Mat img = imgContainer_1.getTemp();
    Mat blurImg;
    blur(img,blurImg,Size(5,5),Point(-1,-1),BORDER_DEFAULT);

    imgUpdate(ui->label_3,img);
    imgContainer_1.push(blurImg);
}

void MainWindow::on_pushButton_hard_clicked()
{
    Mat dstImage = imgContainer_1.getTemp();
    Mat imageBf;
    bilateralFilter(dstImage, imageBf, 5, 10, 10);//双边滤波
    Mat imageEnhance;
    Mat  kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
    filter2D(imageBf, imageEnhance, CV_8UC3, kernel);
    dstImage=imageEnhance;

    imgUpdate(ui->label_3,dstImage);
    imgContainer_1.push(dstImage);
}

void MainWindow::on_pushButton_cut1_clicked()
{
    QImage cutImage = cvMat2QImage(imgContainer_1.getTemp());

    QPixmap pixmap = QPixmap::fromImage(cutImage);

    if(!pixmap.isNull())
    {
        imgCrop* Crop = new imgCrop(pixmap,this);

        int ret=Crop->exec();
        if(ret==QDialog::Accepted){
            ui->label_3->setPixmap(Crop->resultImage.scaled(ui->label_3->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
            cvImg = cvQImage2Mat(Crop->resultImage.toImage());
            imgContainer_1.push(cvImg);
        }
        delete Crop;
    }else {
        QMessageBox::critical(this,"错误信息","请先选择图片");
    }
}

void MainWindow::openImg(QLabel* imgLabel, bool storge)  //选择并打开图像
{
    imgLabel->clear();
    QString OpenFile,OpenFliePath;
    QImage image;
    //getOpenFileName获取文件路径并转化为字符串
    OpenFile = QFileDialog::getOpenFileName(this,
                                            "请选择图片",
                                            "",
                                            "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");
    if(OpenFile != "" && QFile::exists(OpenFile))
    {
        if(image.load(OpenFile))
        {
            OpenFliePath = QFileInfo(OpenFile).filePath();

            Mat img = imread(OpenFliePath.toStdString());

            if(storge)
            {
                cvImg = img;
                imgContainer.push(img);
            }else{
                imgContainer_1.push(img);
            }

            imgUpdate(imgLabel,img);
        }
    }else if(OpenFile.isEmpty()) {
        qDebug()<<"图像失败";
        return;
    }
}

void MainWindow::imgUpdate(QLabel* imgLabel, Mat img) //将图像显示在label中
{
    QImage image = cvMat2QImage(img);

    imgOpened = true;

    imgLabel->setAlignment(Qt::AlignCenter);
    imgLabel->setPixmap(QPixmap::fromImage(image).scaled(imgLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}


QImage MainWindow::cvMat2QImage(const Mat &mat) // 将mat图片转换为QImage
{
    QImage image;
    switch(mat.type())
    {
    case CV_8UC1:
        // QImage构造：数据，宽度，高度，每行多少字节，存储结构
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        break;
    case CV_8UC3:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        image = image.rgbSwapped(); // BRG转为RGB
        // Qt5.14增加了Format_BGR888
        // image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
        break;
    case CV_8UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        break;
    case CV_16UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
        image = image.rgbSwapped(); // BRG转为RGB
        break;
    }
    return image;
}

Mat MainWindow::cvQImage2Mat(const QImage &image) // 将QImage图片转换为mat
{
    Mat img;
    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_Grayscale8:
        // QImage构造：数据，宽度，高度，每行多少字节，存储结构
        img = Mat(image.height(),image.width(),CV_8UC1,const_cast<uchar*>(image.bits()), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        img = Mat(image.height(),image.width(),CV_8UC3,const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cvtColor(img, img, cv::COLOR_RGB2BGR); // RGB转为BGR
        // Qt5.14增加了Format_BGR888
        // image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
        break;
    case QImage::Format_RGB32:
        img = Mat(image.height(),image.width(),CV_8UC4,const_cast<uchar*>(image.bits()), image.bytesPerLine());
        break;
    case QImage::Format_RGBA64:
        img = Mat(image.height(),image.width(),CV_16UC4,const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cvtColor(img, img, cv::COLOR_RGB2BGR); // RGB转为BGR
        break;
    }

    return img.clone();
}
