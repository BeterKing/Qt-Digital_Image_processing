#ifndef IMGCROP_H
#define IMGCROP_H

#include <QDialog>
#include "imagecropperlabel.h"

namespace Ui {
class imgCrop;
}

class imgCrop : public QDialog
{
    Q_OBJECT

public:
    explicit imgCrop(QPixmap pixmap,QWidget *parent = nullptr);
    ~imgCrop();
    void init();
    QPixmap resultImage;

private slots:
    void on_pushButton_clicked();
private:
    Ui::imgCrop *ui;
    QPixmap pixmap;
    ImageCropperLabel* imgCropperLabel;
};

#endif // IMGCROP_H
