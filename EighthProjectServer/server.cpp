#include "server.h"

Server::Server()
{
    db = QSqlDatabase::addDatabase("QSQLITE", "MyConnection_1");
    db.setDatabaseName("./usersDB.db");
    if(!db.open()){
        qDebug() << db.lastError().text();
    }

    query = new QSqlQuery(db);
    query->exec("CREATE TABLE Users(Firstname TEXT, Lastname TEXT, Login TEXT, Password TEXT);");

    if (this->listen(QHostAddress::Any, 2323))
    {
        qDebug() << "Start";
    }
    else
    {
        qDebug() << "Error";
    }
    nextBlockSize = 0;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::close);

    Sockets.push_back(socket);
    qDebug() << "client connected" << socketDescriptor;
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_3);
    if (in.status() == QDataStream::Ok)
    {
        qDebug() << "read...";

        for (;;)
        {
            if (nextBlockSize == 0)
            {
                qDebug() << "nextBlockSize = 0";
                if (socket->bytesAvailable() < 2)
                {
                    qDebug() << "Data < 2, break";
                    break;
                }
                in >> nextBlockSize;
                qDebug() << "nextBlockSize = " << nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize)
            {
                qDebug() << "Data not full, break";
                break;
            }
            QString type, str1, str2, str3, str4;
            in >> type;
            if (type == "authorization")
            {
                in >> str1 >> str2;
                nextBlockSize = 0;
                qDebug() << str1 << str2;
                LoginAndPasswordChecker(str1, str2);
                break;
            }
            else if (type == "registration")
            {
                in >> str1 >> str2 >> str3 >> str4;
                nextBlockSize = 0;
                qDebug() << str1 << str2 << str3 << str4;
                AddNewUser(str1, str2, str3, str4);
                break;
            }
        }
    }
    else
    {
        qDebug() << "DataStream error";
    }
}

void Server::SendToClient(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_3);

    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));

    for (int i = 0; i < Sockets.size(); i++)
    {
        Sockets[i]->write(Data);
    }
}

void Server::SendToClient(QString type, QString str1, QString str2)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_3);

    out << quint16(0) << type << str1 << str2;
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));

    for (int i = 0; i < Sockets.size(); i++)
    {
        Sockets[i]->write(Data);
    }
}

void Server::LoginAndPasswordChecker(QString login, QString password)
{
    query->prepare("SELECT Login From Users Where Login == :login And Password == :password;");
    query->bindValue(":login", login);

    QByteArray bytes = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
    QString digest = QString(bytes.toHex());

    query->bindValue(":password", digest);

    auto request = query->exec();
    query->next();

    if(request && !query->value(0).isNull()){
        query->prepare("SELECT Firstname, Lastname FROM Users Where Login == :login;");
        query->bindValue(":login", login);
        query->exec();
        query->next();

        const QString name = query->value(0).toString();
        const QString sirname = query->value(1).toString();

        qDebug() << name << sirname;

        SendToClient("Success", name, sirname);
    } else {
        SendToClient("NotFound");
    }
}

void Server::AddNewUser(QString firstname, QString lastname, QString login, QString password)
{
    query->prepare("SELECT Firstname FROM Users Where Login == :login;");
    query->bindValue(":login", login);

    if (!query->exec()) {
        return;
    }

    query->next();

    if (!query->value(0).isNull()){
        SendToClient("Taken");
    } else {
        QByteArray bytes = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
        QString digest = QString(bytes.toHex());

        query->prepare("INSERT INTO Users (Firstname, Lastname, Login, Password) VALUES (:firstname, :lastname, :login, :password);");
        query->bindValue(":firstname", firstname);
        query->bindValue(":lastname", lastname);
        query->bindValue(":login", login);
        query->bindValue(":password", digest);
        query->exec();
        SendToClient("Added");
    }
}
