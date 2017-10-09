#ifndef PLAYINGTCPSERVER_H
#define PLAYINGTCPSERVER_H

#include <QTcpServer>
#include <QObject>

class PlayingTCPServer : public QObject
{
    Q_OBJECT
public:
    explicit PlayingTCPServer(int nPort = 7575, QObject *parent = 0);
public slots:
    void onConnection();
protected:
  QTcpServer *m_ptcpServer;
};

#endif // PLAYINGTCPSERVER_H
