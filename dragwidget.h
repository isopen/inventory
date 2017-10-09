#ifndef DRAGWIDGET_H
#define DRAGWIDGET_H

#include <QtWidgets>
#include <QtSql>
#include <QFrame>

class QDragEnterEvent;
class QDropEvent;

class DragWidget : public QFrame
{
public:
    DragWidget(QWidget *parent = 0);
    void setTypeObjectDragWidget(QByteArray type, QString url);
private:
    QLabel *boatIcon;
protected:
    QByteArray typeObject = "apple";
    QString imgObject = ":/images/boat.png";
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DRAGWIDGET_H
