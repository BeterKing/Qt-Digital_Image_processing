#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>
#include <QImage>

using namespace std;
using namespace cv;

template<class T>
class algorithm
{
protected:
    T src;
    T dst;

public:

    int ksize = 5;
    double alpha;
    double beta;
    int channel = 0;

    virtual T process(T img, int param);
    virtual void setSRC(T);
    virtual T getDST();
};

template<class T>
void algorithm<T>::setSRC(T src)
{
    this->src = src;
}

template<class T>
T algorithm<T>::process(T img, int param){
    setSRC(img);
    return getDST();
}

#endif // ALGORITHM_H
