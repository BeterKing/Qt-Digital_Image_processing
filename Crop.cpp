#include "Crop.h"
#include "ui_Crop.h"
#include "ui_mainwindow.h"
#include <imagecropperlabel.h>

Dialog::Dialog(QPixmap pixmap ,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , pixmap(pixmap)
{
    ui->setupUi(this);
    init();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::init() {
    imgCropperLabel = new ImageCropperLabel(600, 400, this);
    imgCropperLabel->setFrameStyle(1);
    imgCropperLabel->setRectCropper();
    imgCropperLabel->setOriginalImage(pixmap);
    imgCropperLabel->update();
}


