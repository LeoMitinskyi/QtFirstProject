#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "secondwindow.h"
#include <QtWidgets>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    form = new Form;
    window = new SecondWindow();

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));

    socket->connectToHost("127.0.0.1", 2323);

    connect(form, &Form::signalRegistrationDataGetter, this, &MainWindow::slotRegistrationDataGetter);
    connect(this, &MainWindow::signalNameGetter, window, &SecondWindow::slotNameGetter);
    connect(this, &MainWindow::signalQJsonArrayGetter, window, &SecondWindow::slotlQJsonArrayGetter);
    connect(this, &MainWindow::signalSocketGetter, window, &SecondWindow::slotSocketGetter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sockDisc()
{
    socket->deleteLater();
}

void MainWindow::on_authButton_clicked()
{
    if (socket->isOpen())
    {
        QString login = ui->loginEdit->text();
        QString password = ui->passwordEdit->text();
        QByteArray ba1 = login.toLocal8Bit();
        QByteArray ba2 = password.toLocal8Bit();
        socket->write("{\"type\":\"auth\", \"login\":\"" + ba1 + "\", \"password\":\"" + ba2 + "\"}");
    } else {
        QMessageBox::information(this, "Information", "Not connected");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    form->show();
}

void MainWindow::slotRegistrationDataGetter(QString name, QString sirname, QString login, QString password)
{
    if (socket->isOpen())
    {
        QByteArray ba1 = name.toLocal8Bit();
        QByteArray ba2 = sirname.toLocal8Bit();
        QByteArray ba3 = login.toLocal8Bit();
        QByteArray ba4 = password.toLocal8Bit();
        socket->write("{\"type\":\"registration\", \"name\":\"" + ba1 + "\", \"sirname\":\"" + ba2 + "\", \"login\":\"" + ba3 + "\", \"password\":\"" + ba4 + "\"}");
        socket->waitForBytesWritten(500);
    } else {
        QMessageBox::information(this, "Information", "Not connected");
    }
}

void MainWindow::sockReady()
{
    if (socket->waitForConnected(500))
    {
        socket->waitForReadyRead(500);
        Data = socket->readAll();

        doc = QJsonDocument::fromJson(Data, &docError);

        if (docError.errorString() == "no error occurred")
        {
            if ((doc.object().value("type").toString() == "connect") && (doc.object().value("status").toString() == "yes"))
            {
                qDebug() << "Connected";
            } else if (doc.object().value("type").toString() == "resultSelect") {
                QJsonArray docAr = doc.object().value("result").toArray();
                emit signalQJsonArrayGetter(docAr);
            } else if ((doc.object().value("type").toString() == "auth") && (doc.object().value("status").toString() == "found")) {
                QMessageBox::information(this, "Information", "Success");
                hide();
                window->setModal(true);
                window->show();
                emit signalNameGetter(doc.object().value("name").toString(), doc.object().value("sirname").toString());
                emit signalSocketGetter(socket);
                if (socket->isOpen())
                {
                    socket->write("{\"type\":\"select\", \"params\":\"ListOfProducts\"}");
                    socket->waitForBytesWritten(500);
                } else {
                    QMessageBox::information(this, "Information", "Not connected");
                }
            } else if ((doc.object().value("type").toString() == "auth") && (doc.object().value("status").toString() == "notfound")) {
                QMessageBox::warning(this, "Notification", "Not found!");
            } else if ((doc.object().value("type").toString() == "registration") && (doc.object().value("status").toString() == "added")) {
                QMessageBox::information(this, "Notification", "User was added!");
            } else if ((doc.object().value("type").toString() == "registration") && (doc.object().value("status").toString() == "taken")) {
                QMessageBox::warning(this, "Notification", "This login is taken!");
            } else {
                QMessageBox::information(this, "Information", "Not connected");
            }
        } else {
            QMessageBox::information(this, "Information", "Format reading error" + docError.errorString());
        }
    }
}
