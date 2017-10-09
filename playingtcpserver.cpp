#include "playingtcpserver.h"

#include <QTcpServer>

PlayingTCPServer::PlayingTCPServer(int nPort, QObject *parent) : QObject(parent),
    m_ptcpServer(new QTcpServer(this))
{
    connect(m_ptcpServer, SIGNAL(newConnection()), SLOT(onConnection()));

    if (false == m_ptcpServer->listen(QHostAddress::LocalHost, nPort)) {
        m_ptcpServer->close();
        throw m_ptcpServer->errorString();
     }
}

void PlayingTCPServer::onConnection() {
    qDebug() << "connection";
}
