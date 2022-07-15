#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QDateTime>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QCryptographicHash>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server();
    QTcpSocket *socket;

private:
    QVector <QTcpSocket*> Sockets;
    QByteArray Data;
    void SendToClient(QString);
    void SendToClient(QString, QString, QString);
    void LoginAndPasswordChecker(QString, QString);
    void AddNewUser(QString, QString, QString, QString);
    quint16 nextBlockSize;
    QSqlDatabase db;
    QSqlQuery *query;
    QSqlTableModel *model;

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

#endif // SERVER_H
