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
    bool loadFile(const QString &fileName);
    void setImage(const QImage &newImage);
    // get list of images in current dir
    QList<QString> getImagesFromDir(const QString &path);

    QList<QString>::iterator findImageOnCurrenPath();

    void createTwoLayots(); // in main widget

    void addImagesToTwoLayouts(QList<QString>::iterator it);
    void setImageToFrame(const QString &path, ClickableLabel* imgLabel);
    // change images in 2 layouts on clicking
    void changeImages(const QString &imagePath);

    void getDirPath();

    void createClickLabelList();

    void fixCurrenImagePath();

    void initialiseWindow();

    QString shortFileName(QString fileName);

    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void scaleImage(double factor);
protected:
    void keyPressEvent(QKeyEvent* e);
    void wheelEvent(QWheelEvent* e);
private slots:

    void on_actionOpen_triggered();

    void on_actionZoom_In_triggered();

    void on_actionZoom_out_triggered();

    void on_actionRestore_picture_s_size_triggered();

private:

    bool isInitialised = false;
    QString dirPath;
    QString currentImagePath;
    QList<ClickableLabel*> clickLabelsList;
    QList<QString> images;
    QString _ARG;
    QRect displayRect;
    QScrollArea *scrollArea;
//    QLabel *imageLabel;
    QImage image;
    Ui::MainWindow *ui;
    QLabel* label;
    QLabel* labelText;
    double scaleFactor = 1;

};
#endif // MAINWINDOW_H
