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
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server();
    ~Server();

    QTcpSocket* socket;
    QByteArray Data;
    QJsonDocument doc;
    QJsonParseError docError;
    QSqlDatabase db;

private:
    void checkLoginAndPassword(QString, QString);
    void addNewUser(QString, QString, QString, QString);
    void addNewItem(QString, QString);
    void deleteItem(QString, QString);

public slots:
    void startServer();
    void incomingConnection(qintptr socketDescription);
    void sockReady();
    void sockDisc();
};

#endif // SERVER_H
