#include "dragwidget.h"

#include <QtWidgets>
#include <QtSql>

#include "esqlquery.h"

DragWidget::DragWidget(QWidget *parent)
    : QFrame(parent)
{
    this->boatIcon = new QLabel(this);
    this->boatIcon->setPixmap(QPixmap(imgObject));
    this->boatIcon->show();
    this->boatIcon->setAttribute(Qt::WA_DeleteOnClose);
    this->boatIcon->setScaledContents(true);

    this->adjustSize();
    this->setAcceptDrops(true);

}

void DragWidget::setTypeObjectDragWidget(QByteArray type, QString url) {
    this->typeObject = type;
    this->imgObject = url;
    this->boatIcon->setPixmap(QPixmap(url));
    ESqlQuery query;
    query.insertIntoItems(QString(type), url);
}

void DragWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void DragWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void DragWidget::mousePressEvent(QMouseEvent *event)
{
    QLabel *child = static_cast<QLabel*>(childAt(event->pos()));
    if (!child)
        return;

    QPixmap pixmap = *child->pixmap();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos() - child->pos());

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
    mimeData->setData("typeObject", this->typeObject);
    mimeData->setData("countObject", "1");

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());

    QPixmap tempPixmap = pixmap;
    QPainter painter;
    painter.begin(&tempPixmap);
    painter.end();

    child->setPixmap(tempPixmap);

    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
        child->close();
    } else {
        child->show();
        child->setPixmap(pixmap);
    }

}
