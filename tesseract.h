#ifndef TESSERACT_H
#define TESSERACT_H
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <regex>
#include <QDebug>

using namespace std;
using namespace cv;

template <class T>
class fontMananger
{
private:
    String outText;
public:
    Mat preprocess(Mat gray);
    vector<RotatedRect> findTextRegion(Mat img);
    Mat Croping(vector<Point2f> points, Mat img);
    vector<Point2f> reorder(vector<Point2f> points);
    Mat grayImages(Mat img);
    string removeExtraSpacesAndNewlines(string str);
};

template<class T>
Mat fontMananger<T>::preprocess(Mat gray)
{
    Mat gaussBlur;
    GaussianBlur(gray, gaussBlur, Size(3, 3),1.5);

    Mat mediaBlur;
    medianBlur(gaussBlur, mediaBlur, 3);

    //Sobel算子，x方向求梯度
    Mat sobel;
    Sobel(mediaBlur, sobel, CV_8U, 1, 0, 3);

    //二值化
    Mat binary;
    threshold(sobel, binary, 0, 255, THRESH_OTSU + THRESH_BINARY);

    //膨胀和腐蚀操作核设定
    Mat element1 = getStructuringElement(MORPH_RECT, Size(3,3));
    //控制高度设置可以控制上下的膨胀程度
    Mat element2 = getStructuringElement(MORPH_RECT, Size(30,15));

    //膨胀一次，让轮廓突出
    Mat dilate1;
    dilate(binary, dilate1, element2);

    //腐蚀一次，去掉细节，表格线等，这里去掉的是竖直的线
    Mat erode1;
    erode(dilate1, erode1, element1);

    //6.再次膨胀，让轮廓明显一些
    Mat dilate2;
    dilate(erode1, dilate2, element2);

    return dilate2;
}

template<class T>
vector<RotatedRect> fontMananger<T>::findTextRegion(Mat img)
{
    vector<RotatedRect> rects;
    //查找轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(img, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0,0));

    //筛选面积小的
    for(size_t i = 0; i < contours.size(); i++)
    {
        //计算当前轮廓的面积
        double area = contourArea(contours[i]);

        //将面积小于1000的全部筛选掉
        if(area < 1000)
            continue;

        //轮廓近似，作用较小，approxPolyDp函数有待研究
        double epsilon = 0.001*arcLength(contours[i], true);
        Mat approx;
        approxPolyDP(contours[i], approx, epsilon, true);

        //找到最小矩形，该矩形可能有方向
        RotatedRect rect = minAreaRect(approx);

        //计算高和宽
        int m_width = rect.boundingRect().width;
        int m_height = rect.boundingRect().height;

        //筛选那些太细的矩形，留下扁的
        if(m_height > m_width*1.2)
            continue;

        //符合条件的rect添加到rects集合中
        rects.push_back(rect);
    }

    return rects;
}

template<class T>
Mat fontMananger<T>::Croping(vector<Point2f> points, Mat img)
{
    //对图像进行切割提高tesseract识别的准确度
    Mat imgCrop;

    float width, height;

    //计算矩形的长和宽
    width = points[1].x - points[0].x;
    height = points[1].y - points[0].y;

    //定义字体轮廓矩形
    Rect roi((int)points[0].x, (int)points[0].y, (int)width, (int)height);

    //保留对应的矩形区域成新的图片，以方便接下来进行处理
    imgCrop = img(roi);

    return imgCrop;

}

template<class T>
vector<Point2f> fontMananger<T>::reorder(vector<Point2f> points)
{
    vector<Point2f> newPoints;
    vector<float> sumPoints, subPoints;

    for(int i=0; i<points.size(); i++)
    {
        sumPoints.push_back(points[i].x + points[i].y);
        subPoints.push_back(points[i].x - points[i].y);
    }

    //找到轮廓矩形的1、4顶点
    newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
    newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);

    return newPoints;

}

template<class T>
Mat fontMananger<T>::grayImages(Mat img)
{
    // 创建灰度图像
    Mat grayImage(img.rows, img.cols, CV_8UC1);

    // 遍历图像像素
    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            // 获取当前像素的 RGB 通道值
            Vec3b pixel = img.at<Vec3b>(y, x);
            int B = pixel[0];
            int G = pixel[1];
            int R = pixel[2];

            // 计算灰度值（加权平均）
            int grayValue = 0.2989 * R + 0.5870 * G + 0.1140 * B;

            // 将灰度值赋给灰度图像
            grayImage.at<uchar>(y, x) = grayValue;
        }
    }
    return grayImage;
}

template<class T>
string fontMananger<T>::removeExtraSpacesAndNewlines(string str)
{
    // 使用正则表达式替换一个或多个空白字符为一个空格
    std::regex re("\\s+");
    str = std::regex_replace(str, re, " ");

    // 移除字符串两端的空格
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);

    return str;
}

#endif // TESSERACT_H
