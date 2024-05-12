#include "imgcrop.h"
#include "ui_imgcrop.h"

imgCrop::imgCrop(QPixmap pixmap ,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::imgCrop)
    , pixmap(pixmap)
{
    ui->setupUi(this);
    init();
    connect(ui->pushButton,&QPushButton::clicked,this,&QDialog::accept);
}

imgCrop::~imgCrop()
{
    delete ui;
}

void imgCrop::init() {
    imgCropperLabel = new ImageCropperLabel(600, 400, this);
    imgCropperLabel->setFrameStyle(1);
    imgCropperLabel->setRectCropper();
    imgCropperLabel->setOriginalImage(pixmap);
    imgCropperLabel->enableOpacity(true);
    imgCropperLabel->setDragSquareEdge(8);
    imgCropperLabel->setDragSquareColor(Qt::green);
}

void imgCrop::on_pushButton_clicked()
{
    resultImage = imgCropperLabel->getCroppedImage();
    this->close();
}
