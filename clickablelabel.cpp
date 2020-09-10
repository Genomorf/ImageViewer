#include "clickablelabel.h"
#include <QDebug>

ClickableLabel::ClickableLabel(const QString& text, QWidget* parent)
    : QLabel(parent)
{
    setText(text);
}

ClickableLabel::~ClickableLabel()
{
}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked(_path);
}

void ClickableLabel::setImagePath(const QString &path)
{
    _path = path;
}
