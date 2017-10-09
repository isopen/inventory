#include "server.h"
#include "serversocketadapter.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>

Server::Server(int nPort, QObject *parent) : QObject(parent),
  m_ptcpServer(new QTcpServer(this)) {

  connect(m_ptcpServer, SIGNAL(newConnection()), SLOT(on_newConnection()));

  if (false == m_ptcpServer->listen(QHostAddress::LocalHost, nPort)) {
    m_ptcpServer->close();
    throw m_ptcpServer->errorString();
  } 
}

void Server::on_newConnection() {

  QTcpSocket* pclientSock = m_ptcpServer->nextPendingConnection();
  ISocketAdapter *pSockHandle = new ServerSocketAdapter(pclientSock);

  m_clients.push_back(pSockHandle);

  //pSockHandle->sendString("connect");

  connect(pSockHandle, SIGNAL(disconnected()), SLOT(on_disconnected()));
  connect(pSockHandle, SIGNAL(message(QString)), SLOT(on_message(QString)));
}

void Server::on_disconnected() {

  ISocketAdapter* client = static_cast<ServerSocketAdapter*>(sender());
  m_clients.removeOne(client);
  delete client;
}

void Server::on_message(QString msg) {
  ISocketAdapter *sock = static_cast<ServerSocketAdapter*>(sender());
  for(int i = 0; i < m_clients.length(); i++) {
    if(sock != m_clients[i]) {
        m_clients[i]->sendString(msg);
    }
  }
}


