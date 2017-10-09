#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSql>
#include <QTcpServer>
#include <QTcpSocket>

#include "server/server.h"
#include "client/clientsocketadapter.h"
#include "esqlquery.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMessageBox msgBox;
    msgBox.setText("Тип приложения");
    msgBox.setInformativeText("Активировать режим сервера?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();

    if(ret == QMessageBox::Yes) {
        new Server(7575, this);
    }

    ClientSocketAdapter *m_pSock = new ClientSocketAdapter(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("inventory.sqlite");
    if (!db.open()) {
        qDebug() << "Не удалось подключиться к базе данных";
    }else {
        ESqlQuery query;
        query.initTables();
    }

    DragWidget *dw = new DragWidget(this);
    dw->setTypeObjectDragWidget("apple", ":/images/boat.png");

    DragWidget *dw1 = new DragWidget(this);
    dw1->setTypeObjectDragWidget("car", ":/images/car.png");
    dw1->setGeometry(300, 0, 200, 200);

    PlayingFieldTableWidget *playingFieldTableWidget = new PlayingFieldTableWidget(3, 3, this);
    playingFieldTableWidget->setClientSocketAdapter(m_pSock);

}

MainWindow::~MainWindow()
{
    delete ui;
}
