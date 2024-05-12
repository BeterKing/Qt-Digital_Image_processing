#ifndef IMGMANANGER_H
#define IMGMANANGER_H

#include <stack>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>
#include <QImage>
using namespace cv;
using namespace std;

template <class T>
class imgMananger
{
private:
    stack<T> dealed;
    stack<T> withdrawed;
    T origin;
public:
    bool isEmpty();              //判断是否为空
    void push(T);                //将处理的图像存入缓存
    void setTemp(T);             //记录某张图像
    T getTemp();                 //获取临时存储的图像
    T back();                    //后退操作
    void cleanDealed();             //清空已处理缓存
};

template <class T>
bool imgMananger<T>::isEmpty()
{
    return dealed.empty();
}

template <class T>
void imgMananger<T>::push(T img)
{
    origin = img;
    dealed.push(img);
}

template <class T>
void imgMananger<T>::setTemp(T img)
{
    origin = img;
}

template <class T>
T imgMananger<T>::getTemp()
{
    return origin;
}

template <class T>
T imgMananger<T>::back()
{
    if(dealed.size()-1)
    {
        dealed.pop();
    }
    return dealed.top();
}

template <class T>
void imgMananger<T>::cleanDealed()
{
    while(dealed.size()-1)
    {
        T tempImg = dealed.top();
        dealed.pop();
        withdrawed.push(tempImg);
    }
}

#endif // IMGMANANGER_H
