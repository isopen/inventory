#include "playingfieldtablewidget.h"

#include <QtWidgets>
#include <QtDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QJsonDocument>
#include "client/clientsocketadapter.h"
#include "esqlquery.h"

PlayingFieldTableWidget::PlayingFieldTableWidget(int rows, int columns, QWidget *parent)
    :QTableWidget(rows, columns, parent)
{

    this->setGeometry(100, 200, this->widthCell * rows + 2, this->heightCell * columns + 2);
    this->horizontalHeader()->hide();
    this->verticalHeader()->hide();
    this->setFocusPolicy(Qt::NoFocus);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setIconSize(QSize(widthCell, heightCell));

    for(int i = 0; i < 3; i++) {
        this->setRowHeight(i, widthCell);
        this->setColumnWidth(i, heightCell);
    }
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setAcceptDrops(true);

    QVector<QJsonArray> initVector;
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            this->setItem(row, column, new QTableWidgetItem);
            initVector.insert(column, {0, ""});
        }
        playFieldVector.insert(row, initVector);
        initVector.clear();
    }

}

void PlayingFieldTableWidget::setClientSocketAdapter(ClientSocketAdapter *sock) {
    this->m_pSock = sock;
    connect(m_pSock, SIGNAL(message(QString)), SLOT(onMessage(QString)));
}

void PlayingFieldTableWidget::onMessage(QString s) {

    QJsonArray jsonArray = QJsonDocument::fromJson(s.toUtf8()).array();
    int row = jsonArray.at(0).toString().toInt();
    int column = jsonArray.at(1).toString().toInt();

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) AS count FROM inventory WHERE num_row = :num_row AND num_column = :num_column");
    query.bindValue(":num_row", row);
    query.bindValue(":num_column", column);
    query.exec();
    query.next();

    if(query.value(0).toInt() == 0) {
        playFieldVector[row][column][0] = 0;
        playFieldVector[row][column][1] = "";
        this->setItem(row, column, new QTableWidgetItem);
        return;
    }

    query.prepare("SELECT inventory.count, items.name, items.url, items.id FROM inventory"
                  " LEFT JOIN items ON(inventory.item = items.id)"
                  " WHERE num_row = :num_row AND num_column = :num_column");
    query.bindValue(":num_row", row);
    query.bindValue(":num_column", column);
    query.exec();

    while(query.next()) {
        playFieldVector[row][column][0] = query.value(0).toInt();
        playFieldVector[row][column][1] = query.value(1).toString();
        QTableWidgetItem *targetItem = item(row, column);
        targetItem->setIcon(QPixmap(query.value(2).toString()));
        targetItem->setText(query.value(0).toString());
        targetItem->setTextAlignment(Qt::AlignBottom);
    }

}

void PlayingFieldTableWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-dnditemdata") && event->mimeData()->hasFormat("typeObject")) {
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

void PlayingFieldTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata") && event->mimeData()->hasFormat("typeObject")) {
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

void PlayingFieldTableWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata") && event->mimeData()->hasFormat("typeObject")) {

        QString typeObject = QString(event->mimeData()->data("typeObject"));
        int oldColumnCountObject = QString(event->mimeData()->data("countObject")).toInt();

        QTableWidgetItem *targetItem = itemAt(event->pos());
        int targetItemRow = targetItem->row();
        int targetItemColumn = targetItem->column();

        if(playFieldVector[targetItemRow][targetItemColumn][1] == typeObject || playFieldVector[targetItemRow][targetItemColumn][1] == "") {

            QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            QPixmap pixmap;
            dataStream >> pixmap;

            int countObject = playFieldVector[targetItemRow][targetItemColumn][0].toInt() + oldColumnCountObject;
            playFieldVector[targetItemRow][targetItemColumn][0] = countObject;
            playFieldVector[targetItemRow][targetItemColumn][1] = typeObject;

            QSqlQuery query;
            query.prepare("SELECT id FROM items WHERE name = :name");
            query.bindValue(":name", typeObject);
            query.exec();
            query.next();
            int idObject = query.value(0).toInt(); // TODO:: перенести в mime

            query.prepare("SELECT COUNT(*) AS count FROM inventory WHERE num_row = :num_row AND num_column = :num_column");
            query.bindValue(":num_row", targetItemRow);
            query.bindValue(":num_column", targetItemColumn);
            query.exec();
            query.next();
            if(query.value(0).toInt() == 0) {
                query.prepare("INSERT INTO inventory (num_row, num_column, item, count)"
                              " VALUES (:num_row, :num_column, :item, :count)");
                query.bindValue(":item", QString::number(idObject));
            }else {
                /**
                 * int QSqlQuery::numRowsAffected() const
                 * Returns the number of rows affected by the result's SQL statement, or -1 if it cannot be determined.
                 * Note that for SELECT statements, the value is undefined; use size() instead. If the query is not active, -1 is returned.
                 * http://doc.qt.io/qt-5/qsqlquery.html#numRowsAffected
                 */
                query.prepare("UPDATE inventory SET count = :count WHERE num_row = :num_row AND num_column = :num_column");
            }
            query.bindValue(":count", QString::number(countObject));
            query.bindValue(":num_row", targetItemRow);
            query.bindValue(":num_column", targetItemColumn);
            query.exec();

            QJsonArray jsonArray = {QString::number(targetItemRow), QString::number(targetItemColumn)};
            QJsonDocument doc;
            doc.setArray(jsonArray);
            m_pSock->sendString(doc.toJson());

            targetItem->setIcon(pixmap);
            targetItem->setText(QString::number(playFieldVector[targetItemRow][targetItemColumn][0].toInt()));
            targetItem->setTextAlignment(Qt::AlignBottom);

            if (event->source() == this) {
                event->setDropAction(Qt::MoveAction);
                event->accept();
            } else {
                event->acceptProposedAction();
            }

        }else {
            this->itemRemove = false;
            qDebug() << "Попробуйте в другую ячейку";
            event->ignore();
        }
    } else {
        event->ignore();
    }
}

void PlayingFieldTableWidget::removeItemPlayingField(QMouseEvent *event, bool multiType) {

    if(this->itemRemove) {

        QTableWidgetItem *targetItem = itemAt(event->pos());

        int targetItemRow = targetItem->row();
        int targetItemColumn = targetItem->column();

        ESqlQuery query;

        if(multiType) {

            query.removeFromInventory(targetItemRow, targetItemColumn);

            playFieldVector[targetItemRow][targetItemColumn][0] = 0;
            playFieldVector[targetItemRow][targetItemColumn][1] = "";
            this->setItem(targetItem->row(), targetItem->column(), new QTableWidgetItem);

        }else {

            query.updateInInventory(targetItemRow, targetItemColumn);

            playFieldVector[targetItemRow][targetItemColumn][0] = playFieldVector[targetItemRow][targetItemColumn][0].toInt() - 1;
            if(playFieldVector[targetItemRow][targetItemColumn][0] != 0) {
                this->item(targetItem->row(), targetItem->column())->setText(QString::number(playFieldVector[targetItemRow][targetItemColumn][0].toInt()));
            }else {

                query.removeFromInventory(targetItemRow, targetItemColumn);

                playFieldVector[targetItemRow][targetItemColumn][1] = "";
                this->setItem(targetItem->row(), targetItem->column(), new QTableWidgetItem);
            }

        }

        QJsonArray jsonArray = {QString::number(targetItemRow), QString::number(targetItemColumn)};
        QJsonDocument doc;
        doc.setArray(jsonArray);
        m_pSock->sendString(doc.toJson());

    }else {
        this->itemRemove = true;
    }

}

void PlayingFieldTableWidget::mousePressEvent(QMouseEvent *event) {

    QTableWidgetItem *targetItem = itemAt(event->pos());

    int targetItemRow = targetItem->row();
    int targetItemColumn = targetItem->column();

    QIcon icon = targetItem->icon();

    if(icon.isNull()) {
        return;
    }

    QPixmap pixmap = targetItem->icon().pixmap(this->widthCell, this->heightCell);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
    mimeData->setData("typeObject", playFieldVector[targetItemRow][targetItemColumn][1].toString().toUtf8());
    mimeData->setData("countObject", QString::number(playFieldVector[targetItemRow][targetItemColumn][0].toDouble()).toUtf8());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(drag->pixmap().width()/2, drag->pixmap().height()/2));

    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
        this->removeItemPlayingField(event);
    } else {
        this->removeItemPlayingField(event, false);
    }
}

bool PlayingFieldTableWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if(mouseEvent->button() == Qt::RightButton) {
            this->removeItemPlayingField(mouseEvent, false);
        }
    }
    return true;
}
