#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include "clickablelabel.h"
#include <QKeyEvent>
#include <QWheelEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& ARG, QWidget *parent = nullptr);
    ~MainWindow();
    // open file from "Open button" and load to center widget
    void open();
    bool loadImageToCenterFrame(const QString &fileName);
    void setImage(const QImage &newImage);
    // get list of images in current dir
    QList<QString> getImagesFromDir(const QString &path);

    QList<QString>::iterator findImageOnCurrenPath();

    void createTwoSideLayots(); // in main widget

    void addImagesToTwoLayouts(QList<QString>::iterator it);
    void setImageToFrame(const QString &path, ClickableLabel* imgLabel);
    // change images in 2 layouts on clicking
    void changeImages(const QString &imagePath);

    void getDirPath();

    void initClickLabelList();

    void fixCurrenImagePath();

    void createTwoSideFrames();

    QString shortFileName(QString fileName);

    void scaleImage(double factor);

    void createCenterFrame();
protected:
    void keyPressEvent(QKeyEvent* e);
    void wheelEvent(QWheelEvent* e);
private slots:

    void on_actionOpen_triggered();

    void on_actionZoom_In_triggered();

    void on_actionZoom_out_triggered();

    void on_actionRestore_picture_s_size_triggered();

private:
    void scaleLabelImageByValue(QLabel* Slabel, int width, int height);
    void scaleImageByImageSize();
    void scaleImageByFrameSize();
    bool isInitialised = false;
    QString dirPath;
    QString currentImagePath;
    QList<ClickableLabel*> clickLabelsList;
    QList<QString> images;
    QString _ARG;
    QRect displayRect;
    QScrollArea *scrollArea;
    QImage image;
    Ui::MainWindow *ui;
    QLabel* labelCenterImage;
    QLabel* labelTextTop;
    double scaleFactor = 1;

};
#endif // MAINWINDOW_H
