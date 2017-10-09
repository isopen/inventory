#include "clientsocketadapter.h"
#include <QVector>
#include <QTcpSocket>
#include <QDataStream>
#include <QHostAddress>

ClientSocketAdapter::ClientSocketAdapter(QObject *parent)
  : SocketAdapter(parent) {
  m_ptcpSocket->connectToHost(QHostAddress::LocalHost, 7575);
}
