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


MainWindow::MainWindow(const QString& ARG, QWidget *parent)
    : QMainWindow(parent), _ARG(ARG), imageLabel(new QLabel),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    QVBoxLayout* layoutCenter2 = new QVBoxLayout();
    labelText = new QLabel();
    labelText->setText("CURRENT IMAGE: ");
    QFont font;
    font.setBold(true);
    font.setPixelSize(15);
    labelText->setFont(font);
    layoutCenter2->addWidget(labelText, Qt::AlignCenter);
    ui->frame_3->setLayout(layoutCenter2);
    ui->frame_3->layout()->setAlignment(Qt::AlignCenter);
    label = new QLabel();
    QVBoxLayout* layoutCenter = new QVBoxLayout();
    layoutCenter->addWidget(label, Qt::AlignCenter);
    ui->frame->setLayout(layoutCenter);

    qDebug() << "TEXT" << ui->menubar->windowIconText();

    createClickLabelList();

    displayRect  = QGuiApplication::primaryScreen()->geometry();
    setFixedSize(displayRect.width() -300, displayRect.height() -200);


    #ifdef QT_NO_DEBUG
    if (!_ARG.isEmpty()){
        loadFile(_ARG);

        initialiseWindow();

    }
    #else
    currentImagePath = "C:/Users/Alex/Downloads/FcgABhNEY98.jpg";
    loadFile(currentImagePath);
    initialiseWindow();

    #endif


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialiseWindow()
{
    if(!isInitialised){
        fixCurrenImagePath();

        getDirPath();
        images = getImagesFromDir(dirPath);

        createTwoLayots();

        auto foundImage = findImageOnCurrenPath();

        addImagesToTwoLayouts(foundImage);
        isInitialised = true;
    }
}

void MainWindow::fixCurrenImagePath()
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
{
    qDebug() << "Trying to get dir path...";
    dirPath = currentImagePath;
    for (auto it = currentImagePath.rbegin(); (*it != "/" && *it != "\\"); ++it){
        dirPath.chop(1);
    }
    qDebug() << "Get dir path: " << dirPath;
}

void MainWindow::createClickLabelList()
{
    ClickableLabel* imgLabel1 = new ClickableLabel();
    ClickableLabel* imgLabel2 = new ClickableLabel();
    clickLabelsList.push_back(imgLabel1);
    clickLabelsList.push_back(imgLabel2);
    qDebug() << "Clickable list created";
}

QList<QString>::iterator MainWindow::findImageOnCurrenPath()
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
    connect(imgLabel, &ClickableLabel::clicked, this, &MainWindow::changeImages);

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
    qDebug() << "Trying to change images";
    currentImagePath = imagePath;
    qDebug() << currentImagePath;

    auto foundImage = findImageOnCurrenPath();
    loadFile(*foundImage);

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


void MainWindow::createTwoLayots()
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

QList<QString> MainWindow::getImagesFromDir(const QString &path){
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

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
    if (!currentImagePath.isEmpty()){
        initialiseWindow();

        getDirPath();
        images = getImagesFromDir(dirPath);
        changeImages(currentImagePath);
    }
}

bool MainWindow::loadFile(const QString &fileName)
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
    labelText->setText("CURRENT IMAGE: " + shortFileName(fileName));

    qDebug() << "Loaded file to center widget";
    return true;
}

QString MainWindow::shortFileName(QString fileName){
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
        label->setPixmap(QPixmap::fromImage(
                                 image.scaled(
                                     displayRect.width() - 100,
                                     displayRect.height() - 250,
                                     Qt::KeepAspectRatio)
                                 )
                             );
    } else {
        label->setPixmap(QPixmap::fromImage(image));
    }
    label->setAlignment(Qt::AlignCenter);
    qDebug() << "Set image to center widget";
}
void MainWindow::on_actionOpen_triggered()
{
    open();

}
