#ifndef SCANNER_H
#define SCANNER_H
#include <opencv2/highgui.hpp> // 说是说gui 具体什么gui 不清楚
#include <opencv2/imgcodecs.hpp> // 图像头文件
#include <opencv2/imgproc.hpp> // 图像处理头文件
using namespace std;
using namespace cv;

template <class T>
class Scanner
{
private:

public:
    Mat preProcessing(Mat& img); //图像预处理函数
    vector<Point> getContours(Mat imgDil, Mat& imgOriginal);// 获取轮廓函数

    void drawPoints(vector<Point> points, Scalar color, Mat imgOriginal);// 描点函数

    vector<Point> reorders(vector<Point> initalPoints); // 修改点的顺序，改为想要的 0 1 2 3

    Mat getWarp(Mat imgOriginal, vector<Point> finalPoints, float w, float h);
};
template <class T>
Mat Scanner<T>::preProcessing(Mat& img)
{
    Mat imgGray;
    cvtColor(img, imgGray, COLOR_BGR2GRAY); // OPencv 中含有BGR 协议而不是RGB协议、
    // 高斯滤噪 模糊
    Mat imgBlur;
    GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0); // 该Size 图像具体哪里来的，不是很清楚
    // Canny 边缘检测， 在进行Canny边缘检测之前我们一般会进行高斯滤波
    Mat imgCanny;
    Canny(imgBlur, imgCanny, 25, 75); // 后面两个参数即Canny 边缘检测的阈值

    // How to dilate and erode an image
    // dilate : increase the thickness to convenietly detect  即增加边线的厚度，使其显示的更加明显
    // erode :  decrease the thickness
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3)); // 更改Size 的大小便更改了放大倍数 注意Size 只能选择奇数
    Mat imgDil;
    dilate(imgCanny, imgDil, kernel); // 使用getStructruingElement 创建kernel
    return imgDil;
}

template <class T>
vector<Point> Scanner<T>::getContours(Mat imgDil, Mat& imgOriginal)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy; // Vec4i 即代表该向量内有4个 int 变量typedef    Vec<int, 4>   Vec4i;   这四个向量每一层级代表一个轮廓
    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // CV_CHAIN_APPROX_SIMPLE - 简单的链式接近法
    vector<vector<Point>> conpoly(contours.size());// conpoly(paprameter1) ,paprameter1便代表vector对象的行数，而其列数中的vector 是使用了point点集但其只包含图形的拐角点集
    vector<Point> biggest; // 定义最大轮廓
    // 为了滤除微小噪声，因此计算area 的面积
    int maxArea = 0; // 遍历临时变量，用以查找最大轮廓
    for (int i = 0; i < contours.size(); i++) // 关于contours.size()为什么是返回二维数组的行，因为 vector::size()函数只接受vector 对象的调用而contours的所有行(不管列)均为其对象
    {
        int area = contourArea(contours[i]);
        //cout << area << endl;

        // area > 1000 的目的是为了滤除图像中的微小瑕疵
        if (area > 1000)
        {


            float peri = arcLength(contours[i], true);// 该函数计算轮廓的长度，后面的bool值表面轮廓曲线是否闭合若为true 则轮廓曲线闭合

            approxPolyDP(contours[i], conpoly[i], 0.02 * peri, true); //  conpoly[i]是输出array   0.02*peri 这个参数理解不了就不要理解！！！ 最后一个参数仍然是询问是否闭合

            // 判断图像中最大的矩形
            if (area > maxArea && conpoly[i].size() == 4) // 若面积最大，则将其点值赋值给biggest 其中 biggest 为 point 向量
            {
                //drawContours(imgOriginal, conpoly, i, Scalar(255, 0, 255), 2); // 只需要绘制最大图形即可
                maxArea = area;
                biggest = { conpoly[i][0],conpoly[i][1],conpoly[i][2],conpoly[i][3] };

            }



            //rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
        }

    }
    return biggest;
}

template <class T>
void Scanner<T>::drawPoints(vector<Point> points, Scalar color, Mat imgOriginal)
{
    for (int i = 0; i < points.size(); i++)
    {
        circle(imgOriginal, points[i], 10, color, FILLED);
        putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
    }
}

template <class T>
// vector 容器pushback 函数是在vector 容器的尾部添加元素
vector<Point> Scanner<T>::reorders(vector<Point> initalPoints)
{
    vector<Point> tempPoints; // 设置中间变量
    vector<double> sumPoints, subPoints;
    int sumMinIndex, sumMaxIndex; //  查找sum的最大(3)最小值(0)索引
    int subMinIndex, subMaxIndex; // 查找sub的最大()最小值()索引
    for (int i = 0; i < 4; i++)
    {
        sumPoints.push_back(initalPoints[i].x + initalPoints[i].y); // 和值最大为第四个点
        //和值最小为第一个点
        // 而 2 3 点则是通过y - x 进行判断
        subPoints.push_back(initalPoints[i].x - initalPoints[i].y);
    }
    sumMinIndex = min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin(); // 0
    subMaxIndex = max_element(subPoints.begin(), subPoints.end()) - subPoints.begin(); //1
    subMinIndex = min_element(subPoints.begin(), subPoints.end()) - subPoints.begin();
    sumMaxIndex = max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin();// 3
    tempPoints.push_back(initalPoints[sumMinIndex]); // 0 即最小元素
    tempPoints.push_back(initalPoints[subMaxIndex]);
    tempPoints.push_back(initalPoints[subMinIndex]);
    tempPoints.push_back(initalPoints[sumMaxIndex]);
    return tempPoints;
}

template <class T>
Mat Scanner<T>::getWarp(Mat imgOriginal, vector<Point> finalPoints, float w, float h)
{
    Point2f src1[4] = { finalPoints[0],finalPoints[1],finalPoints[2],finalPoints[3] };
    Point2f src2[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };
    Mat martix = getPerspectiveTransform(src1, src2);
    Mat imgWarp;
    warpPerspective(imgOriginal, imgWarp, martix, Size(w, h));
    return imgWarp;
}


#endif // SCANNER_H
