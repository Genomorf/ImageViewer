#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImageReader>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QApplication>
#include <QStandardPaths>
#include <QImageWriter>
#include <QColorSpace>
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QCommandLineParser>
#include <QGraphicsPixmapItem>
#include <QDirIterator>
#include <QVBoxLayout>
#include <QFont>
#include "clickablelabel.h"
#include <QScrollBar>
#include <QKeyEvent>

MainWindow::MainWindow(const QString& ARG, QWidget *parent)
    : QMainWindow(parent), _ARG(ARG),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // create empty center frame without image
    createCenterFrame();

    // list with 2 ClickableLabel class items
    initClickLabelList();

    displayRect  = QGuiApplication::primaryScreen()->geometry();
    setFixedSize(displayRect.width() -300, displayRect.height() -200);

    #ifdef QT_NO_DEBUG
    if (!_ARG.isEmpty()){
        loadImageToCenterFrame(_ARG);
        createTwoSideFrames();
    }
    #else
    currentImagePath = "C:/Users/Alex/Downloads/FcgABhNEY98.jpg";
    loadImageToCenterFrame(currentImagePath);
    createTwoSideFrames();
    #endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createCenterFrame()
{
    QFont font;
    font.setBold(true);
    font.setPixelSize(15);

    labelTextTop = new QLabel();
    labelTextTop->setText("CURRENT IMAGE: ");
    labelTextTop->setFont(font);

    // setting text in the top of center frame above centerImage
    QVBoxLayout* layoutTop = new QVBoxLayout();
    layoutTop->addWidget(labelTextTop, Qt::AlignCenter);
    ui->frame_3->setLayout(layoutTop);
    ui->frame_3->layout()->setAlignment(Qt::AlignCenter);

    labelCenterImage = new QLabel();

    // setting centerImage in the center of the center frame
    QVBoxLayout* layoutCenter = new QVBoxLayout();
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(labelCenterImage);
    layoutCenter->addWidget(scrollArea, Qt::AlignCenter);
    ui->frame->setLayout(layoutCenter);
}

void MainWindow::scaleLabelImageByValue(QLabel* inputLabel, int width, int height)
{
    inputLabel->setPixmap(QPixmap::fromImage(
                            image.scaled(
                                width,
                                height,
                                Qt::KeepAspectRatio)
                            )
                        );
}

void MainWindow::scaleImage(double factor)
{
    scaleFactor *= factor;

    // prevent to over zoom the image which cause a lot of lags
    if (scaleFactor > 8){
        scaleFactor -= 1;
        ui->statusbar->showMessage("Maximum zoom!");
        return;
    } else if (scaleFactor < 0.01){
        scaleFactor += 0.05;
        ui->statusbar->showMessage("Minimum zoom!");
        return;
    }

    // constants for easier code reading
    const int IMAGE_WIDTH  = image.size().width();
    const int IMAGE_HEIGHT = image.size().height();
    const int FRAME_WIDTH  = ui->frame->geometry().width();
    const int FRAME_HEIGHT = ui->frame->geometry().height();

    // scaling image if it's bigger than frame geometry
    if (IMAGE_WIDTH > FRAME_WIDTH || IMAGE_HEIGHT > FRAME_HEIGHT){

        scaleLabelImageByValue(labelCenterImage, IMAGE_WIDTH * scaleFactor, IMAGE_WIDTH * scaleFactor);

    } else {

        scaleLabelImageByValue(labelCenterImage, IMAGE_WIDTH * scaleFactor, IMAGE_WIDTH * scaleFactor);
    }
}

void MainWindow::on_actionZoom_In_triggered()
// Zoom In button
{
    scaleImage(1.25);
}

void MainWindow::on_actionZoom_out_triggered()
// Zoom Out button
{
    scaleImage(0.75);
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    // nothing yet here
}

void MainWindow::wheelEvent(QWheelEvent* event)
// ctrl + mouse_wheel will zoom in or zoom out centerImage
{
    if (event->angleDelta().y() > 0){
        if(event->modifiers() & Qt::ControlModifier){
            scaleImage(1.05);
        }
    } else if (event->angleDelta().y() < 0){
        if(event->modifiers() & Qt::ControlModifier){
            scaleImage(0.95);
        }
    }
}
void MainWindow::createTwoSideFrames()
{
    // init only once
    if(!isInitialised){

        fixCurrenImagePath();

        getDirPath();
        images = getImagesFromDir(dirPath);

        // creating 2 frames with prev and next images + text labels
        createTwoSideLayots();

        auto foundImage = findImageOnCurrenPath();

        addImagesToTwoLayouts(foundImage);
        isInitialised = true;
    }
}

void MainWindow::fixCurrenImagePath()
// fast way to create working path
// maybe in docs exist more correct method
{
    int counter = 0;
    for (auto& j : currentImagePath){
        if (j == '\\'){
            j = '/';
            ++counter;
        }
    }
    if (counter > 0){
        qDebug() << "Image path fixed";
    }
}

void MainWindow::getDirPath()
// get path of current directory by image path
{
    qDebug() << "Trying to get dir path...";
    dirPath = currentImagePath;
    for (auto it = currentImagePath.rbegin(); (*it != "/" && *it != "\\"); ++it){
        dirPath.chop(1);
    }
    qDebug() << "Get dir path: " << dirPath;
}

void MainWindow::initClickLabelList()
{
    ClickableLabel* imgLabel1 = new ClickableLabel();
    ClickableLabel* imgLabel2 = new ClickableLabel();
    clickLabelsList.push_back(imgLabel1);
    clickLabelsList.push_back(imgLabel2);
    qDebug() << "Clickable list created";
}

QList<QString>::iterator MainWindow::findImageOnCurrenPath()
// QList<QString> images = getImagesFromDir(dirPath);
{
    qDebug() << "Trying to find image in dir...";
    auto foundImage = std::find(images.begin(), images.end(), currentImagePath);
    if (foundImage != images.end()){
        qDebug() << "Image in dir found";
    }
    else {
        qDebug() << "Could not find image in dir";
    }
    return foundImage;
}

void MainWindow::setImageToFrame(const QString &path, ClickableLabel* imgLabel)
{
    qDebug() << "Trying to set image to frame...";
    QImageReader reader(path);
    QImage img = reader.read();
    imgLabel->setImagePath(path);
    // when image is clicked function change prev and next images
    connect(imgLabel, &ClickableLabel::clicked, this, &MainWindow::changeImages);
    //scaleLabelImageByValue
    imgLabel->setPixmap(QPixmap::fromImage(
                            img.scaled(
                                ui->frame_1->maximumSize().width(),
                                ui->frame_1->maximumSize().height(),
                                Qt::KeepAspectRatio)
                            )
                        );
    qDebug() << "Image set to frame";
}

void MainWindow::changeImages(const QString &imagePath)
{
    scaleFactor = 1;
    qDebug() << "Trying to change images";
    currentImagePath = imagePath;
    qDebug() << currentImagePath;

    auto foundImage = findImageOnCurrenPath();
    loadImageToCenterFrame(*foundImage);

    QImageReader reader(foundImage + 1 == images.end() ?
                            *(images.begin()) :
                            *(foundImage + 1));
    QImage img = reader.read();
    clickLabelsList[0]->setImagePath(foundImage + 1 == images.end() ?
                                         *(images.begin()) :
                                         *(foundImage + 1));
    clickLabelsList[0]->setPixmap(QPixmap::fromImage(
                            img.scaled(
                                ui->frame_1->maximumSize().width(),
                                ui->frame_1->maximumSize().height(),
                                Qt::KeepAspectRatio)
                            )
                        );
    QImageReader reader2(foundImage == images.begin() ?
                             images.last() :
                             *(foundImage - 1));
    QImage img2 = reader2.read();
    clickLabelsList[1]->setImagePath(foundImage == images.begin() ?
                                         images.last() :
                                         *(foundImage - 1));
    clickLabelsList[1]->setPixmap(QPixmap::fromImage(
                            img2.scaled(
                                ui->frame_1->maximumSize().width(),
                                ui->frame_1->maximumSize().height(),
                                Qt::KeepAspectRatio)
                            )
                        );
    qDebug() << "Images have changed";
}

void MainWindow::addImagesToTwoLayouts(QList<QString>::iterator it)\
{
    qDebug() << "Trying to add default images to layouts";
    setImageToFrame(it + 1 == images.end() ?
                        *(images.begin()) :
                        *(it + 1), clickLabelsList[0]);
    setImageToFrame(it == images.begin() ?
                        images.last() :
                        *(it - 1), clickLabelsList[1]);
    qDebug() << "Default images added to layouts";
}


void MainWindow::createTwoSideLayots()
{
    QVBoxLayout *layout1 = new QVBoxLayout();
    QVBoxLayout *layout2 = new QVBoxLayout();

    auto textLabel1 = new QLabel();
    auto textLabel2 = new QLabel();

    QFont font;
    font.setPixelSize(15);
    font.setBold(true);

    textLabel1->setText("PREVIOUS IMAGE");
    textLabel1->setFont(font);
    textLabel2->setText("NEXT IMAGE");
    textLabel2->setFont(font);

    layout1->addWidget(textLabel1);
    layout1->addWidget(clickLabelsList[0], Qt::AlignCenter);
    layout1->setAlignment(textLabel1, Qt::AlignCenter);

    layout2->addWidget(textLabel2);
    layout2->addWidget(clickLabelsList[1], Qt::AlignCenter);
    layout2->setAlignment(textLabel2, Qt::AlignCenter);

    ui->frame_1->setLayout(layout1);
    ui->frame_2->setLayout(layout2);
    ui->frame_1->layout()->setAlignment(Qt::AlignCenter);
    ui->frame_2->layout()->setAlignment(Qt::AlignCenter);

    qDebug() << "Created 2 layouts";
}

QList<QString> MainWindow::getImagesFromDir(const QString &path)
{
    qDebug() << "Trying to get list of images from dir...";

    QStringList filters;
    for (auto &i : QImageReader::supportedImageFormats()){
       filters << "*." + i;
    }
    QDir dir(path);
    auto list = dir.entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot, QDir::SortFlag::Time);

    for (int i = 0; i < list.size(); ++i) {
           QFileInfo fileInfo = list.at(i);
           images.push_back(fileInfo.filePath());
    }

    qDebug() << "Got list of images ";
    return images;
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    for (const QByteArray &mimeTypeName : supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void MainWindow::open()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadImageToCenterFrame(dialog.selectedFiles().first())) {}
    if (!currentImagePath.isEmpty()){
        createTwoSideFrames();

        getDirPath();
        images = getImagesFromDir(dirPath);
        changeImages(currentImagePath);
    }
}

bool MainWindow::loadImageToCenterFrame(const QString &fileName)
{
    qDebug() << "Trying to load file to center widget...";
    currentImagePath = fileName;
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    setImage(newImage);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());

    statusBar()->showMessage(message);
    ui->frame->layout()->setAlignment(Qt::AlignCenter);
    labelTextTop->setText("CURRENT IMAGE: " + shortFileName(fileName));

    qDebug() << "Loaded file to center widget";
    return true;
}

QString MainWindow::shortFileName(QString fileName)
{
    QString shortName;
    for (auto it = fileName.rbegin(); *it != "/" && *it != "\\"; ++it){
        shortName.push_front(*it);
    }
    return shortName;
}
void MainWindow::setImage(const QImage &newImage)
{
    qDebug() << "Trying to set image to center widget...";
    image = newImage;
    if (image.colorSpace().isValid())
        image.convertToColorSpace(QColorSpace::SRgb);
    if (image.size().width() > displayRect.width() || image.size().height() > displayRect.height()){
        labelCenterImage->setPixmap(QPixmap::fromImage(
                                 image.scaled(
                                     displayRect.width() - 100,
                                     displayRect.height() - 250,
                                     Qt::KeepAspectRatio)
                                 )
                             );
    } else {
        labelCenterImage->setPixmap(QPixmap::fromImage(image));
    }
    labelCenterImage->setAlignment(Qt::AlignCenter);
    qDebug() << "Set image to center widget";
}
void MainWindow::on_actionOpen_triggered()
{
    open();

}




void MainWindow::on_actionRestore_picture_s_size_triggered()
{
    if (image.size().width() > ui->frame->geometry().width()
            || image.size().height() > ui->frame->geometry().height()){
        scaleLabelImageByValue(labelCenterImage, displayRect.width() - 100,
                               displayRect.height() - 250);
        labelCenterImage->setPixmap(QPixmap::fromImage(
                                 image.scaled(
                                 displayRect.width() - 100,
                                 displayRect.height() - 250,
                                     Qt::KeepAspectRatio)
                                 )
                             );
    } else {
        scaleLabelImageByValue(labelCenterImage, image.size().width(),
                               image.size().width());
    }
}
