#include "server.h"

Server::Server(){}

Server::~Server(){}

void Server::checkLoginAndPassword(QString login, QString password)
{
    QByteArray bytes = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
    QString digest = QString(bytes.toHex());

    QSqlQuery* query = new QSqlQuery(db);
    query->prepare("SELECT Firstname, Lastname From Users Where Login == :login And Password == :password;");
    query->bindValue(":login", login);
    query->bindValue(":password", digest);

    qDebug() << digest;

    if (query->exec()) {
        query->next();
        if (!query->value(0).isNull()){
        const QString name = query->value(0).toString();
        const QString sirname = query->value(1).toString();
        qDebug() << name << sirname;
        QByteArray ba1 = name.toLocal8Bit();
        QByteArray ba2 = sirname.toLocal8Bit();
        socket->write("{\"type\":\"auth\",\"status\":\"found\",\"name\":\"" + ba1 + "\",\"sirname\":\"" + ba2 + "\"}");
        } else {
            qDebug() << "Not found";
            socket->write("{\"type\":\"auth\",\"status\":\"notfound\"}");
        }
    } else {
        qDebug() << "db error";
    }
}

void Server::addNewUser(QString name, QString sirname, QString login, QString password)
{
    QSqlQuery* query = new QSqlQuery(db);
    query->prepare("SELECT Firstname FROM Users Where Login == :login;");
    query->bindValue(":login", login);

    if (query->exec()){
        query->next();
        if (query->value(0).isNull()) {
            QByteArray bytes = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
            QString digest = QString(bytes.toHex());
            query->prepare("INSERT INTO Users (Firstname, Lastname, Login, Password) VALUES (:firstname, :lastname, :login, :password);");
            query->bindValue(":firstname", name);
            query->bindValue(":lastname", sirname);
            query->bindValue(":login", login);
            query->bindValue(":password", digest);
            query->exec();
            qDebug() << "Added";
            socket->write("{\"type\":\"registration\",\"status\":\"added\"}");
        } else {
            qDebug() << "Taken";
            socket->write("{\"type\":\"registration\",\"status\":\"taken\"}");
        }
    }
}

void Server::addNewItem(QString name, QString weight)
{
    QSqlQuery* query = new QSqlQuery(db);
    query->prepare("INSERT INTO ListOfProducts (Name, Weight) VALUES (:name, :weight);");
    query->bindValue(":name", name);
    query->bindValue(":weight", weight);
    if (query->exec()) {
        qDebug() << "New item was added";
    } else {
        qDebug() << "db error";
    }
}

void Server::deleteItem(QString name, QString weight)
{
    QSqlQuery* query = new QSqlQuery(db);
    query->prepare("DELETE FROM ListOfProducts Where Name = :name And Weight = :weight;");
    query->bindValue(":name", name);
    query->bindValue(":weight", weight);
    if (query->exec()) {
        qDebug() << "Item was deleted";
    } else {
        qDebug() << "db error";
    }
}

void Server::startServer()
{
    if (this->listen(QHostAddress::Any, 2323)){
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("usersDB.db");
        if (db.open()){
            qDebug() << "Listening and Open db";
        } else {
            qDebug() << "db not open";
        }
    } else {
        qDebug() << "Not listening";
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));

    qDebug() << "Client connected:  " << socketDescriptor;

    socket->write("{\"type\":\"connect\",\"status\":\"yes\"}");
    qDebug() << "Send to client connect status - YES";
}

void Server::sockDisc()
{
    qDebug() << "Disconnect";
    socket->deleteLater();
}

void Server::sockReady()
{
    Data = socket->readAll();
    qDebug() << "Data: " << Data;

    doc = QJsonDocument::fromJson(Data, &docError);

    if (docError.errorString()=="no error occurred")
    {
        if ((doc.object().value("type").toString() == "select") && (doc.object().value("params").toString() == "ListOfProducts"))
        {
            if (db.isOpen())
            {
                QByteArray result = "{\"type\":\"resultSelect\",\"result\":[";
                QSqlQuery* query = new QSqlQuery(db);
                if (query->exec("SELECT Name, Weight FROM ListOfProducts;"))
                {
                    while (query->next())
                    {
                        result.append("{\"Name\":\"" + query->value(0).toByteArray() + "\"},");
                        result.append("{\"Weight\":\"" + query->value(1).toByteArray() + "\"},");
                    }
                    result.remove(result.length() - 1, 1);
                    result.append("]}");

                    socket->write(result);
                    socket->waitForBytesWritten(500);
                } else {
                    qDebug() << "Query not success";
                }
            }
        } else if (doc.object().value("type").toString() == "auth") {
            if (db.open()){
                auto login =  doc.object().value("login").toString();
                auto password =  doc.object().value("login").toString();
                checkLoginAndPassword(login, password);
            }
        } else if (doc.object().value("type").toString() == "registration") {
            if (db.open()){
                auto name =  doc.object().value("name").toString();
                auto sirname =  doc.object().value("sirname").toString();
                auto login =  doc.object().value("login").toString();
                auto password =  doc.object().value("password").toString();
                addNewUser(name, sirname, login, password);
            }
        } else if (doc.object().value("type").toString() == "insertItem") {
            if (db.open()){
                auto name =  doc.object().value("name").toString();
                auto weight =  doc.object().value("weight").toString();
                addNewItem(name, weight);
            }
        } else if (doc.object().value("type").toString() == "deleteItem") {
            if (db.open()){
                auto name =  doc.object().value("name").toString();
                auto weight =  doc.object().value("weight").toString();
                deleteItem(name, weight);
            }
        }
    }
}
