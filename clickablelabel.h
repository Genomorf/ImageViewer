#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>

class ClickableLabel : public QLabel
{
Q_OBJECT
public:
    explicit ClickableLabel( const QString& text="", QWidget* parent=0 );
    ~ClickableLabel();

    void setImagePath(const QString &path);
signals:
    void clicked(const QString &imagePath);
protected:
    void mousePressEvent(QMouseEvent* event);
private:
    QString _path;
};

#endif
