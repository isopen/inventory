#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QTableWidget>
#include <QVector>
#include <QJsonArray>

#include "client/clientsocketadapter.h"

class PlayingFieldTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    PlayingFieldTableWidget(int rows, int columns, QWidget *parent); // количество ячеек
    void setClientSocketAdapter(ClientSocketAdapter *sock);
private:
    QVector<QVector<QJsonArray>> playFieldVector; // сколько и каких предметов находится
    int widthCell = 200; // ширина ячейки
    int heightCell = 200; // высота ячейки
    bool itemRemove = true;
    ClientSocketAdapter *m_pSock;
private slots:
    void onMessage(QString s);
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void removeItemPlayingField(QMouseEvent *event, bool multiType = true);
};

#endif // MYWIDGET_H
