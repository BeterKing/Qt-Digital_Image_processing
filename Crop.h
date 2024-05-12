#ifndef CROP_H
#define CROP_H

#include <QDialog>
#include "imagecropperlabel.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QPixmap pixmap,QWidget *parent = nullptr);
    ~Dialog();
    init();

private:
    Ui::Dialog *ui;
    QPixmap pixmap;
    ImageCropperLabel* imgCropperLabel;
};

#endif // CROP_H
